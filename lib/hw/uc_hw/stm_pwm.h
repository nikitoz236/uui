#pragma once

#include <stdint.h>
#include "pwm.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

struct pwm_cfg {
    TIM_TypeDef * tim;
    hw_pclk_t tim_pclk;
    gpio_pin_t gpio;
    uint8_t ch;
    uint16_t max_val;
    unsigned freq;
};
