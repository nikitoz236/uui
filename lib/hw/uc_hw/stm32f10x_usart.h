#pragma once
#include <stdint.h>
#include "api_usart.h"
#include "gpio_stm32f10x.h"
#include "rcc_stm32f10x.h"
#include "stm32f10x.h"

struct usart_cfg {
    USART_TypeDef * usart;
    uint32_t default_baud;
    gpio_pin_t rx;
    gpio_pin_t tx;
    hw_pclk_t pclk;
    uint8_t irqn;
    // uint8_t dma_tx_ch;
    // void (*rx_byte_handler)(uint8_t c);
    // void (* handler)(void);

};
