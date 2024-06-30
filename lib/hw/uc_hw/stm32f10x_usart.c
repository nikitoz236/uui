#include "stm32f10x_usart.h"
#include "str_utils.h"
#include "dma.h"
#include "irq_vectors.h"

#define __DBGPIO_USART_WAIT_AVAILABLE(x)
#define __DBGPIO_DMA_IRQ(x)
#define __DBGPIO_DMA_IRQ_END_BUF(x)
#define __DBGPIO_USART_START_DMA(x)

const gpio_cfg_t rx_pin_cfg = {
    .mode = GPIO_MODE_INPUT,
    .pull = GPIO_PULL_NONE,
};

const gpio_cfg_t tx_pin_cfg = {
    .mode = GPIO_MODE_AF,
    .speed = GPIO_SPEED_HIGH,
    .type = GPIO_TYPE_PP,
};

void dma_usart1_tx_handler(void);
const usart_cfg_t * usart1_cfg;

void usart_set_cfg(const usart_cfg_t * usart)
{
    hw_rcc_pclk_ctrl(&usart->pclk, 1);

    usart->usart->CR1 = 0;
    usart->usart->CR2 = 0;
    usart->usart->CR3 = 0;

    // if (usart->rx.port != 0) {
        gpio_set_cfg(&usart->rx_pin, &rx_pin_cfg);
        usart->usart->CR1 |= USART_CR1_RE;
    // }

    // if (usart->tx.port != 0) {
        gpio_set_cfg(&usart->tx_pin, &tx_pin_cfg);
        usart->usart->CR1 |= USART_CR1_TE;

        if (usart->tx_dma.size != 0) {
            usart->tx_dma.ctx->data_pos = 0;
            usart->tx_dma.ctx->next_tx = 0;
            usart1_cfg = usart;
            dma_channel(usart->tx_dma.dma_ch)->CCR = 0;
            dma_channel(usart->tx_dma.dma_ch)->CCR |= DMA_CCR1_MINC;
            dma_channel(usart->tx_dma.dma_ch)->CCR |= DMA_CCR1_TCIE;
            dma_set_periph_tx(usart->tx_dma.dma_ch, (void *)&usart->usart->DR);
            NVIC_SetHandler(DMA1_Channel1_IRQn + usart->tx_dma.dma_ch, dma_usart1_tx_handler);
            NVIC_EnableIRQ(DMA1_Channel1_IRQn + usart->tx_dma.dma_ch);
            usart->usart->CR3 |= USART_CR3_DMAT;
        }
    // }

    usart_set_baud(usart, usart->default_baud);

    usart->usart->CR1 |= USART_CR1_UE;
}

void usart_set_baud(const usart_cfg_t * usart, unsigned baud)
{
    usart->usart->BRR = hw_rcc_f_pclk(usart->pclk.bus) / baud;
}

static inline void usart_tx_byte(const usart_cfg_t * usart, char c)
{
    while ((usart->usart->SR & USART_SR_TXE) == 0) {};
    usart->usart->DR = c;
}

static inline void usart_tx_blocking(const usart_cfg_t * usart, const void * data, unsigned len)
{
    while(len--) {
        usart_tx_byte(usart, *(char *)data);
        data++;
    }
}

void usart_tx_dma_ringbuf(const usart_cfg_t * usart, const void * data, unsigned len);

void usart_tx(const usart_cfg_t * usart, const void * data, unsigned len)
{
    usart_tx_dma_ringbuf(usart, data, len);
    // usart_tx_blocking(usart, data, len);
}

unsigned usart_is_tx_in_progress(const usart_cfg_t * usart);

unsigned usart_is_rx_available(const usart_cfg_t * usart);
void usart_rx(const usart_cfg_t * usart, void * data, unsigned len);


void usart_dma_tx_end(const usart_cfg_t * usart)
{
    __DBGPIO_DMA_IRQ(1);
    dma_clear_irq_full(usart->tx_dma.dma_ch);

    dma_stop(usart->tx_dma.dma_ch);

    const unsigned buf_size = usart->tx_dma.size;
    if (usart->tx_dma.ctx->next_tx == buf_size) {
        __DBGPIO_DMA_IRQ_END_BUF(1);
        usart->tx_dma.ctx->next_tx = 0;
    }

    void * tx_ptr = &usart->tx_dma.ctx->data[usart->tx_dma.ctx->next_tx];
    unsigned tx_len = 0;

    if (usart->tx_dma.ctx->data_pos > usart->tx_dma.ctx->next_tx) {
        tx_len = usart->tx_dma.ctx->data_pos - usart->tx_dma.ctx->next_tx;
    } else if (usart->tx_dma.ctx->data_pos < usart->tx_dma.ctx->next_tx) {
        tx_len = buf_size - usart->tx_dma.ctx->next_tx;
    }
    if (tx_len) {
        dma_start(usart->tx_dma.dma_ch, tx_ptr, tx_len);
        usart->tx_dma.ctx->next_tx += tx_len;
    }
    __DBGPIO_DMA_IRQ_END_BUF(0);
    __DBGPIO_DMA_IRQ(0);
}

void dma_usart1_tx_handler(void)
{
    // проблема. нам нужны здесь некие универсальные хэндлеры которые могут для нескольких усартов подтягивать контекст.
    usart_dma_tx_end(usart1_cfg);
}

static inline unsigned calc_availabe(unsigned size, unsigned next_tx, unsigned data_pos, unsigned dma_cnt)
{
    unsigned available = next_tx;
    if (available < data_pos) {
        available += size;
    }
    available -= data_pos - dma_cnt;
    return available;
}

void usart_tx_dma_ringbuf(const usart_cfg_t * usart, const void * data, unsigned len)
{
    const unsigned buf_size = usart->tx_dma.size;
    while (len) {
        unsigned len_for_load = len;
        if (len_for_load > buf_size) {
            // небольшая оптимизация. нам важно чтобы ядро освободилось как можно раньше,
            // значит нужно оставить на потом кратное размеру буфера количество данных
            len_for_load %= buf_size;
            if (len_for_load == 0) {
                len_for_load = buf_size;
            }
        }

        __DBGPIO_USART_WAIT_AVAILABLE(1);
        unsigned dma_cnt = dma_get_cnt(usart->tx_dma.dma_ch);

        // по сути нужно вычислить 2 значения:
        //  available - сколько мы сейчас данных можем переложить в буфер
        //  len_for_load - сколько данных мы будем перекладывать
        //  cp_len_end - какая часть из этого копируется в конец до переполнения индекса

        unsigned available = usart->tx_dma.ctx->next_tx;
        if (available <= usart->tx_dma.ctx->data_pos) {
            available += buf_size;
        }
        available -= usart->tx_dma.ctx->data_pos;
        available -= dma_cnt;

        if (available < len_for_load) {
            // блокируемся
            continue;
        }

        __DBGPIO_USART_WAIT_AVAILABLE(0);

        unsigned cp_len_end = buf_size - usart->tx_dma.ctx->data_pos;
        if (cp_len_end > len_for_load) {
            cp_len_end = len_for_load;
        }

        uint8_t * cp_ptr = &usart->tx_dma.ctx->data[usart->tx_dma.ctx->data_pos];
        str_cp(cp_ptr, data, cp_len_end);

        if (cp_len_end < len_for_load) {
            unsigned cp_len = len_for_load - cp_len_end;
            str_cp(usart->tx_dma.ctx->data, &(((uint8_t *)data)[cp_len_end]), cp_len);
        }

        unsigned new_data_pos = usart->tx_dma.ctx->data_pos + len_for_load;
        if (new_data_pos >= buf_size) {
            new_data_pos -= buf_size;
        }

        data += len_for_load;
        len -= len_for_load;

        dma_disable_irq_full(usart->tx_dma.dma_ch);
        usart->tx_dma.ctx->data_pos = new_data_pos;
        if (!dma_is_active(usart->tx_dma.dma_ch)) {
            __DBGPIO_USART_START_DMA(1);
            dma_start(usart->tx_dma.dma_ch, &usart->tx_dma.ctx->data[usart->tx_dma.ctx->next_tx], cp_len_end);
            usart->tx_dma.ctx->next_tx += cp_len_end;
            // обработка переполнения next_tx производится в прерывании
            __DBGPIO_USART_START_DMA(0);
        }
        dma_clear_irq_full(usart->tx_dma.dma_ch);
        dma_enable_irq_full(usart->tx_dma.dma_ch);
    }
}
