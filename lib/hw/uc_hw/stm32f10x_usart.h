#pragma once
#include <stdint.h>
#include "stm32f10x.h"
#include "usart.h"
#include "gpio.h"
#include "hw_rcc.h"

// TODO: ringbuf ?
struct dma_tx_ctx {
    uint16_t data_pos;
    uint16_t next_tx;
    uint8_t data[];
};

struct rb {
    uint16_t head;
    uint16_t tail;
    uint8_t data[];
};

struct dma_rb_desc {
    struct rb * ctx;
    uint16_t size;
    uint8_t dma_ch;
};

// TODO: rx too ?
struct dma_tx_desc {
    struct dma_tx_ctx * ctx;
    uint16_t size;
    uint8_t dma_ch;
};

struct usart_cfg {
    USART_TypeDef * usart;
    uint32_t default_baud;
    struct dma_tx_desc tx_dma;
    struct dma_rb_desc rx_dma;
    gpio_pin_cfg_t * rx_pin;        // тут указатели изза того что не известна реализация структур надо подумать как сэкономить место
    gpio_pin_cfg_t * tx_pin;
    hw_pclk_t * pclk;
    uint8_t irqn;

    // uint8_t dma_tx_ch;
    // void (*rx_byte_handler)(uint8_t c);
    // void (* handler)(void);
};
