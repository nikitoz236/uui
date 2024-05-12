#include "stm32f10x_usart.h"

const gpio_cfg_t rx_pin_cfg = {
    .mode = GPIO_MODE_INPUT,
    .pull = GPIO_PULL_NONE,
};

const gpio_cfg_t tx_pin_cfg = {
    .mode = GPIO_MODE_AF,
    .speed = GPIO_SPEED_HIGH,
    .type = GPIO_TYPE_PP,
};

void usart_set_cfg(const usart_cfg_t * usart)
{
    hw_rcc_pclk_ctrl(&usart->pclk, 1);

    usart->usart->CR1 = 0;
    usart->usart->CR2 = 0;
    usart->usart->CR3 = 0;

    // if (usart->rx.port != 0) {
        gpio_set_cfg(&usart->rx, &rx_pin_cfg);
        usart->usart->CR1 |= USART_CR1_RE;
    // }

    // if (usart->tx.port != 0) {
        gpio_set_cfg(&usart->tx, &tx_pin_cfg);
        usart->usart->CR1 |= USART_CR1_TE;
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
