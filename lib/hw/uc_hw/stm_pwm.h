#pragma once

#include <stdint.h>
#include "periph_header.h"
#include "pwm.h"
#include "pclk.h"
#include "gpio.h"

struct pwm_cfg {
    TIM_TypeDef * tim;
    gpio_t * gpio;
    pclk_t pclk;
    unsigned freq;
    uint16_t max_val;
    uint8_t ch;     // 1 ... 4 like in datasheet
    uint8_t ch_n;   // 0 or 1 for N ch
};
