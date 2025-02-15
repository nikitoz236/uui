#include "periph_header.h"
#include "stm_usart.h"
#include "str_utils.h"
#include "stm_dma.h"
#include "irq_vectors.h"

#define __DBGPIO_USART(n, x);

// extern gpio_pin_t debug_gpio_list[];
// #define __DBGPIO_USART(n, x)                gpio_set_state(&debug_gpio_list[n], x)

#define __DBGPIO_USART_WAIT_AVAILABLE(x)    //  __DBGPIO_USART(1, x)
#define __DBGPIO_DMA_IRQ(x)                 //  __DBGPIO_USART(2, x)
#define __DBGPIO_DMA_IRQ_END_BUF(x)         //  __DBGPIO_USART(0, x)
#define __DBGPIO_USART_START_DMA(x)         //  __DBGPIO_USART(0, x)
#define __DBGPIO_USART_TX_FUNC(x)           __DBGPIO_USART(1, x)
#define __DBGPIO_USART_TX_DMA_WAIT_PREV(x)  __DBGPIO_USART(2, x)

static inline void usart_tx_byte(const usart_cfg_t * usart, char c)
{
    while ((usart->usart->ISR & USART_ISR_TXE) == 0) {};
    usart->usart->TDR = c;
}

static inline void usart_tx_blocking(const usart_cfg_t * usart, const void * data, unsigned len)
{
    while(len--) {
        usart_tx_byte(usart, *(char *)data);
        data++;
    }
}

static inline void usart_tx_dma(const usart_cfg_t * usart, const void * data, unsigned len)
{
    unsigned dma_ch = usart->tx_dma.dma_ch;
    if (dma_is_active(dma_ch)) {
        __DBGPIO_USART_TX_DMA_WAIT_PREV(1);
        while (dma_get_cnt(dma_ch) != 0) {};
        __DBGPIO_USART_TX_DMA_WAIT_PREV(0);
        dma_stop(dma_ch);
    }
    dma_start(dma_ch, data, len);
}

unsigned usart_is_tx_in_progress(const usart_cfg_t * usart);



void usart_tx_dma_rb(const usart_cfg_t * usart, const void * data, unsigned len)
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

        unsigned available = usart->tx_dma.rb->tail;
        if (available <= usart->tx_dma.rb->head) {
            available += buf_size;
        }
        available -= usart->tx_dma.rb->head;
        available -= dma_cnt;

        if (available < len_for_load) {
            // блокируемся
            continue;
        }

        __DBGPIO_USART_WAIT_AVAILABLE(0);

        unsigned cp_len_end = buf_size - usart->tx_dma.rb->head;
        if (cp_len_end > len_for_load) {
            cp_len_end = len_for_load;
        }

        uint8_t * cp_ptr = &usart->tx_dma.rb->data[usart->tx_dma.rb->head];
        str_cp(cp_ptr, data, cp_len_end);

        if (cp_len_end < len_for_load) {
            unsigned cp_len = len_for_load - cp_len_end;
            str_cp(usart->tx_dma.rb->data, &(((uint8_t *)data)[cp_len_end]), cp_len);
        }

        unsigned new_data_pos = usart->tx_dma.rb->head + len_for_load;
        if (new_data_pos >= buf_size) {
            new_data_pos -= buf_size;
        }

        data += len_for_load;
        len -= len_for_load;

        dma_disable_irq_full(usart->tx_dma.dma_ch);
        usart->tx_dma.rb->head = new_data_pos;
        if (!dma_is_active(usart->tx_dma.dma_ch)) {
            __DBGPIO_USART_START_DMA(1);

            // !!! исправлена ошибка !!!
            // непонял зачем я стираю флаг висящего прерывания после зпуска дма ?
            // возникала ситуация когда cp_len_end = 1 ( ну видимо tail был равен buf_size - 1)
            // соответственно прерывание мгновенно взводилось и тут же сбрасывалось до разрешения. обработчик не вызывался.
            // в обработчике должен вызываться dma_stop чтобы можно было снова попасть сюла через dma_is_active
            // и так транзакции дма больше не начинались. буфер забивался и в какойто момент мы блокировались на проверке available
            // перенес сброс флагов до фактического запуска дма

            dma_clear_irq_full(usart->tx_dma.dma_ch);
            dma_start(usart->tx_dma.dma_ch, &usart->tx_dma.rb->data[usart->tx_dma.rb->tail], cp_len_end);
            usart->tx_dma.rb->tail += cp_len_end;
            // обработка переполнения tail производится в прерывании
            __DBGPIO_USART_START_DMA(0);
        }


        dma_enable_irq_full(usart->tx_dma.dma_ch);
    }
}

void usart_dma_tx_end_irq_handler(const usart_cfg_t * usart)
{
    // в прерывании мы оказываемся когда dma досчитает до 0
    // в такой момент нужно либо запустить новую транзакцию если накидали еще данных
    // либо остановиться
    // также здесь происходит обработка перехода через конец буффера в начало
    __DBGPIO_DMA_IRQ(1);
    dma_clear_irq_full(usart->tx_dma.dma_ch);

    dma_stop(usart->tx_dma.dma_ch);

    const unsigned buf_size = usart->tx_dma.size;
    if (usart->tx_dma.rb->tail == buf_size) {
        __DBGPIO_DMA_IRQ_END_BUF(1);
        usart->tx_dma.rb->tail = 0;
    }

    void * tx_ptr = &usart->tx_dma.rb->data[usart->tx_dma.rb->tail];
    unsigned tx_len = 0;

    if (usart->tx_dma.rb->head > usart->tx_dma.rb->tail) {
        tx_len = usart->tx_dma.rb->head - usart->tx_dma.rb->tail;
    } else if (usart->tx_dma.rb->head < usart->tx_dma.rb->tail) {
        tx_len = buf_size - usart->tx_dma.rb->tail;
    }

    if (tx_len) {
        dma_start(usart->tx_dma.dma_ch, tx_ptr, tx_len);
        usart->tx_dma.rb->tail += tx_len;
    }
    __DBGPIO_DMA_IRQ_END_BUF(0);
    __DBGPIO_DMA_IRQ(0);
}

void usart_set_baud(const usart_cfg_t * usart, unsigned baud)
{
    usart->usart->BRR = pclk_f(&usart->pclk) / baud;
}

void usart_set_cfg(const usart_cfg_t * usart)
{
    pclk_ctrl(&usart->pclk, 1);

    usart->usart->CR1 = 0;
    usart->usart->CR2 = 0;
    usart->usart->CR3 = 0;

    if (usart->usart_irq_handler) {
        NVIC_SetHandler(usart->irqn, usart->usart_irq_handler);
        NVIC_EnableIRQ(usart->irqn);
    }

    if (usart->rx_pin != 0) {
        init_gpio(usart->rx_pin);
        usart->usart->CR1 |= USART_CR1_RE;

        if (usart->rx_byte_handler) {
            usart->usart->CR1 |= USART_CR1_RXNEIE;
        } else if (usart->rx_dma.dma_ch != 0) {
            unsigned dma_rx_ch = usart->rx_dma.dma_ch;
            dma_channel(dma_rx_ch)->CCR = 0;
            dma_enable_mem_inc(dma_rx_ch);
            dma_set_periph_rx(dma_rx_ch, (void *)&usart->usart->RDR);
            usart->usart->CR3 |= USART_CR3_DMAR;
            if (usart->rx_dma.size != 0) {

            }
        }
    }

    if (usart->tx_pin != 0) {
        init_gpio(usart->tx_pin);
        usart->usart->CR1 |= USART_CR1_TE;

        if (usart->tx_dma.dma_ch != 0) {
            unsigned dma_tx_ch = usart->tx_dma.dma_ch;
            dma_channel(dma_tx_ch)->CCR = 0;
            dma_enable_mem_inc(dma_tx_ch);
            dma_set_periph_tx(dma_tx_ch, (void *)&usart->usart->TDR);
            usart->usart->CR3 |= USART_CR3_DMAT;

            if (usart->tx_dma.size != 0) {
                usart->tx_dma.rb->head = 0;
                usart->tx_dma.rb->tail = 0;
                if (usart->tx_dma_irq_handler) {
                    dma_set_handler(dma_tx_ch, usart->tx_dma_irq_handler);
                    dma_enable_irq_full(dma_tx_ch);
                    dma_enable_nvic_irq(dma_tx_ch);
                }
            }
        }
    }

    usart_set_baud(usart, usart->default_baud);

    usart->usart->CR1 |= USART_CR1_UE;
}

void usart_tx(const usart_cfg_t * usart, const void * data, unsigned len)
{
    __DBGPIO_USART_TX_FUNC(1);
    if (usart->tx_dma.dma_ch != 0) {
        if (usart->tx_dma.size != 0) {
            usart_tx_dma_rb(usart, data, len);
        } else {
            usart_tx_dma(usart, data, len);
        }
    } else {
        usart_tx_blocking(usart, data, len);
    }
    __DBGPIO_USART_TX_FUNC(0);
}

void usart_rx(const usart_cfg_t * usart, void * data, unsigned len)
{
    unsigned dma_rx_ch = usart->rx_dma.dma_ch;
    dma_stop(dma_rx_ch);
    dma_start(dma_rx_ch, data, len);
}

unsigned usart_is_rx_available(const usart_cfg_t * usart)
{
    if (dma_get_cnt(usart->rx_dma.dma_ch)) {
        return 0;
    }
    return 1;
}

void usart_irq_handler(const usart_cfg_t * usart)
{
    if (usart->usart->ISR & USART_ISR_RXNE) {
        if (usart->rx_byte_handler) {
            usart->rx_byte_handler(usart->usart->RDR);
        }
    }
}
