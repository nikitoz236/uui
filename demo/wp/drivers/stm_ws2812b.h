#pragma once

#include "periph_header.h"
#include "pclk.h"
#include "periph_gpio.h"
#include "gpio.h"

typedef struct {
    TIM_TypeDef * tim;
    gpio_t gpio;
    pclk_t tim_pclk;
    uint8_t tim_ch;
    uint8_t dma_ch;
    uint8_t dma_irq;
} ws_cfg_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ws_rgb_t;

void init_ws(const ws_cfg_t * cfg);
void ws_update(const ws_rgb_t * colors, unsigned len);
