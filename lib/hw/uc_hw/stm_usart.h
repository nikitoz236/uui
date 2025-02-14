#pragma once
#include <stdint.h>
#include "usart.h"
#include "gpio.h"
#include "pclk.h"
#include "rb.h"

struct dma_rb_desc {
    rb_t * rb;
    uint16_t size;
    uint8_t dma_ch;
};

struct usart_cfg {
    USART_TypeDef * usart;
    gpio_t * rx_pin;        // тут указатели изза того что не известна реализация структур надо подумать как сэкономить место
    gpio_t * tx_pin;
    uint32_t default_baud;

    struct dma_rb_desc tx_dma;
    struct dma_rb_desc rx_dma;
    pclk_t pclk;
    uint8_t irqn;
    union {
        void (*tx_dma_handler)(void);
        void (*tx_empty_handler)(void);
    };
};

void usart_dma_tx_end_handler(const usart_cfg_t * usart);
