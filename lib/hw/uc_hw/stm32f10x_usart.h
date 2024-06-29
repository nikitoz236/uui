#pragma once
#include <stdint.h>
#include "usart.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

struct dma_tx_ctx {
    uint16_t data_pos;
    uint16_t next_tx;
    uint8_t data[];
};

struct dma_tx_desc {
    struct dma_tx_ctx * ctx;
    uint16_t size;
    uint8_t dma_ch;
};

struct usart_cfg {
    USART_TypeDef * usart;
    uint32_t default_baud;
    struct dma_tx_desc tx_dma;
    gpio_pin_t rx_pin;
    gpio_pin_t tx_pin;
    hw_pclk_t pclk;
    uint8_t irqn;

    // uint8_t dma_tx_ch;
    // void (*rx_byte_handler)(uint8_t c);
    // void (* handler)(void);
};

