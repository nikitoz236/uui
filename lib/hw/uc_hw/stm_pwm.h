#pragma once

#include <stdint.h>
#include "periph_header.h"
#include "pwm.h"
#include "pclk.h"
#include "gpio.h"

struct pwm_cfg {
    gpio_pin_cfg_t * gpio;
    TIM_TypeDef * tim;
    pclk_t tim_pclk;
    uint8_t ch;
    uint16_t max_val;
    unsigned freq;
};
