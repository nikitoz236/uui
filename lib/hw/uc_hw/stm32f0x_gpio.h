#pragma once
#include <stdint.h>
#include "periph_header.h"
#include "gpio.h"

struct gpio_cfg {
    enum {
        GPIO_MODE_INPUT     = 0,
        GPIO_MODE_OUTPUT    = 1,
        GPIO_MODE_AF        = 2,
        GPIO_MODE_ANALOG    = 3,
    } mode : 2;
    enum {
        GPIO_SPEED_LOW      = 0b00,
        GPIO_SPEED_MED      = 0b01,
        GPIO_SPEED_HIGH     = 0b11,
    } speed : 2;
    enum {
        GPIO_PULL_NONE      = 0,
        GPIO_PULL_UP        = 1,
        GPIO_PULL_DOWN      = 2,
    } pull : 2;
    enum {
        GPIO_TYPE_PP        = 0,
        GPIO_TYPE_OD        = 1,
    } type : 2;
    uint8_t af : 4;
};

struct gpio_pin_cfg {
    gpio_pin_t gpio;
    gpio_cfg_t cfg;
};
