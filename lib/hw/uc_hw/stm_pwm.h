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
    uint8_t ch;
};
