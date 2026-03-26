#pragma once
#include <stdint.h>

#include "soc/i2s_struct.h"
#include "esp32_gpio.h"
#include "esp32_pclk.h"

typedef struct {
    i2s_dev_t * dev;
    pclk_t pclk;
    const gpio_t * mclk;
    const gpio_t * bclk;
    const gpio_t * ws;
    const gpio_t * dout;
    uint8_t clk_sel;    /* 0=XTAL, 1=PLL240M, 2=PLL160M */
    uint8_t mclk_div;   /* MCLK = clk_src / mclk_div */
    uint8_t bck_div;    /* BCLK = MCLK / bck_div */
    uint8_t bits;       /* бит на семпл: 16, 24, 32 */
} i2s_cfg_t;

void init_i2s(const i2s_cfg_t * cfg);
