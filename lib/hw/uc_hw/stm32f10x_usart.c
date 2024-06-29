#include "stm32f10x_usart.h"
#include "str_utils.h"
#include "dma.h"
#include "irq_vectors.h"

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
            usart1_cfg = usart;
            dma_channel(usart->tx_dma.dma_ch)->CCR = 0;
            dma_channel(usart->tx_dma.dma_ch)->CCR |= DMA_CCR1_MINC;
            dma_set_periph_tx(usart->tx_dma.dma_ch, (void *)&usart->usart->DR);
            NVIC_SetHandler(DMA1_Channel1_IRQn + usart->tx_dma.dma_ch, dma_usart1_tx_handler);
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

void usart_tx(const usart_cfg_t * usart, const void * data, unsigned len)
{

    usart_tx_blocking(usart, data, len);
}

unsigned usart_is_tx_in_progress(const usart_cfg_t * usart);

unsigned usart_is_rx_available(const usart_cfg_t * usart);
void usart_rx(const usart_cfg_t * usart, void * data, unsigned len);


void usart_dma_tx_end(const usart_cfg_t * usart)
{
    dma_stop(usart->tx_dma.dma_ch);

    const unsigned buf_size = usart->tx_dma.size;
    void * tx_ptr = &usart->tx_dma.ctx->data[usart->tx_dma.ctx->next_tx];
    unsigned tx_len = 0;
    if (usart->tx_dma.ctx->next_tx == buf_size) {
        usart->tx_dma.ctx->next_tx = 0;
    }
    if (usart->tx_dma.ctx->data_pos > usart->tx_dma.ctx->next_tx) {
        tx_len = usart->tx_dma.ctx->data_pos - usart->tx_dma.ctx->next_tx;
    } else if (usart->tx_dma.ctx->data_pos < usart->tx_dma.ctx->next_tx) {
        tx_len = buf_size - usart->tx_dma.ctx->next_tx;
    }
    dma_start(usart->tx_dma.dma_ch, tx_ptr, tx_len);
    usart->tx_dma.ctx->next_tx += tx_len;
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
        unsigned dma_cnt = dma_get_cnt(usart->tx_dma.dma_ch);

        // по сути нужно вычислить 2 значения:
        //  available - сколько мы сейчас данных переложим в буфер
        //  cp_len_end - какая часть из этого копируется в конец до переполнения индекса

        unsigned available = usart->tx_dma.ctx->next_tx;
        if (available < usart->tx_dma.ctx->data_pos) {
            available += buf_size;
        }
        available -= usart->tx_dma.ctx->data_pos - dma_cnt;

        if (available == 0) {
            // блокируемся
            continue;
        }

        unsigned cp_len_end = buf_size - usart->tx_dma.ctx->data_pos;
        if (cp_len_end > available) {
            cp_len_end = available;
        }

        uint8_t * cp_ptr = &usart->tx_dma.ctx->data[usart->tx_dma.ctx->data_pos];
        str_cp(cp_ptr, data, cp_len_end);

        if (cp_len_end < available) {
            unsigned cp_len = available - cp_len_end;
            str_cp(usart->tx_dma.ctx->data, &(((uint8_t *)data)[cp_len_end]), cp_len);
        }

        unsigned new_data_pos = usart->tx_dma.ctx->data_pos + available;
        if (new_data_pos >= buf_size) {
            new_data_pos -= buf_size;
        }

        data += available;
        len -= available;

        dma_disable_irq_full(usart->tx_dma.dma_ch);
        usart->tx_dma.ctx->data_pos = new_data_pos;
        if (!dma_is_active(usart->tx_dma.dma_ch)) {
            dma_start(usart->tx_dma.dma_ch, &usart->tx_dma.ctx->data[usart->tx_dma.ctx->next_tx], cp_len_end);
            usart->tx_dma.ctx->next_tx += cp_len_end;
            // обработка переполнения next_tx производится в прерывании
        }
        dma_clear_irq_full(usart->tx_dma.dma_ch);
        dma_enable_irq_full(usart->tx_dma.dma_ch);
    }
}

// void usart_tx_dma_ringbuf(const usart_cfg_t * usart, const void * data, unsigned len)
// {
//     /*
//     что тут происходит ? 

//     у нас 2 варианта. либо передача уже идет, либо нет

//         если не идет то мы кладем в начало и запускаем транзакцию
//         если идет то мы из контекста знаем в какое место буффера положить новые данные
//         может быть вариант когда буффер переполнен, тогда блокируемся

//         самая быстрая стратегия - оставить максимальное количество данных  но не больше чем размер буффера на последнюю транзакцию.

//     соответственно у нас есть прерывание по завершению транзакции
//     в результате может быть 3 состояния

//         если данных больше нет то просто останавливаемся
//         есть еще данные - нужно продолжить отправку начиная с промежуточного положения в буффере - нужно отправлять не больше чем до конца буффера
//         если это конец буффера - проверяем есть ли данные и перезапускаемся

//     пример:
//     буффер 64 байта
//     next_tx = 0
//     data_pos = 0

//     прилетело 50 байт
//     сдвинули data_pos на 50
//     в буффере свободного места 64 - 50 = 14 байт
//     потом запустили транзакцию на 50 байт. начиная с next_tx, стало быть после запуска next_tx = 50

//     потом прилетело еще 4 байта
//     сдвинули data_pos на 4 = 54
//     */

//     unsigned cp_availabe = usart->tx_dma.size - usart->tx_dma.ctx->data_pos;

//     while (len) {

//         unsigned cp_len = cp_availabe;
//         if (cp_len > len) {
//             cp_len = len;
//         }

//         uint8_t * cp_ptr = &usart->tx_dma.ctx->data[usart->tx_dma.ctx->data_pos];
//         str_cp(cp_ptr, data, cp_len);
//         usart->tx_dma.ctx->data_pos += cp_len;
//         if (usart->tx_dma.ctx->data_pos == usart->tx_dma.size) {
//             usart->tx_dma.ctx->data_pos = 0;
//         }
//         len -= cp_len;
//         data += cp_len;

//         // нужен ли здесь атомик ???


//         if (!dma_active()) {
//             dma_start(&usart->tx_dma.ctx->data[usart->tx_dma.ctx->next_tx], usart->tx_dma.ctx->data_pos - usart->tx_dma.ctx->next_tx);
//             usart->tx_dma.ctx->next_tx = usart->tx_dma.ctx->data_pos;
//         }

//         if (len) {
//             unsigned data_remain = len;
//             if (data_remain > usart->tx_dma.size) {
//                 data_remain %= usart->tx_dma.size;
//             }

//             unsigned dma_cnt_wait = usart->tx_dma.ctx->next_tx - data_remain;
//             while (dma_get_cnt() > dma_cnt_wait) {};

//             usart->tx_dma.ctx->data_pos = 0;
//         }



//     while (dma_active()) {};



//     if (dma_remaining == 0) {


//         dma_start(usart->tx_dma.ctx->data, cp_len);
//         usart->tx_dma.ctx->next_tx = cp_len;
//     } else {
//         unsigned cp_len = usart->tx_dma.size - usart->tx_dma.ctx->data_pos; // осталось до конца буффера
//         if (cp_len > len) {
//             cp_len = len;
//         }

//         str_cp(&usart->tx_dma.ctx->data[usart->tx_dma.ctx->data_pos], data, cp_len);
//         usart->tx_dma.ctx->data_pos = cp_len;
//         len -= cp_len;
//         data += cp_len;

//     }

//     if (len) {
//         // блокируемся
//     }

//     if (dma_active()) {

//         unsigned free_space = usart->tx_dma.size + usart->tx_dma.ctx->next_tx - usart->tx_dma.ctx->data_pos - dma_remaining;

//         // next_tx - место откуда будет запускаться следующая транзакиця (хвост буффера)
//         // data_pos - место куда мы можем класть новые данные
//         unsigned free_space = usart->tx_dma.ctx->size - usart->tx_dma.ctx->data_pos + usart->tx_dma.ctx->next_tx;


//     } 




//     uint8_t * wr_ptr = usart->tx_dma.ctx->data;

//     if (dma_active()) {
//         wr_ptr += usart->tx_dma.ctx->tx_pos;
//     } 


//     if (dma_active()) {
//         wr_ptr += 
//         str_cp(wr_ptr, data, len);
        
//     }
//         str_cp();
//         start_dma();
//     }
// }



