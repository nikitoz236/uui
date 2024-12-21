#pragma once
#include <stdint.h>
#include "stm32f10x.h"
#include "usart.h"
#include "gpio.h"
#include "hw_rcc.h"
#include "rb.h"

struct dma_rb_desc {
    rb_t * rb;
    uint16_t size;
    uint8_t dma_ch;
};

struct usart_cfg {
    USART_TypeDef * usart;
    uint32_t default_baud;
    struct dma_rb_desc tx_dma;
    struct dma_rb_desc rx_dma;
    gpio_pin_cfg_t * rx_pin;        // тут указатели изза того что не известна реализация структур надо подумать как сэкономить место
    gpio_pin_cfg_t * tx_pin;
    hw_pclk_t * pclk;
    uint8_t irqn;

    // uint8_t dma_tx_ch;
    // void (*rx_byte_handler)(uint8_t c);
    // void (* handler)(void);
};
