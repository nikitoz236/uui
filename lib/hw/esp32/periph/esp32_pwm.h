#pragma once

#include <stdint.h>
#include "gpio.h"

struct pwm_cfg {
    gpio_t * gpio;
    uint8_t out_ch;
    uint8_t tim_ch;
    uint8_t duty_res;
};
