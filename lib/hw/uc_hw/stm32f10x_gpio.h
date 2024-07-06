#pragma once
#include "gpio.h"
#include <stdint.h>

struct gpio_cfg {
    enum {
        GPIO_MODE_ANALOG,
        GPIO_MODE_INPUT,
        GPIO_MODE_OUTPUT,
        GPIO_MODE_AF,
    } mode : 2;
    enum {
        GPIO_SPEED_LOW = 0b10,
        GPIO_SPEED_MED = 0b01,
        GPIO_SPEED_HIGH = 0b11,
    } speed : 2;
    enum {
        GPIO_TYPE_PP,
        GPIO_TYPE_OD,
    } type : 2;
    enum {
        GPIO_PULL_NONE,
        GPIO_PULL_UP,
        GPIO_PULL_DOWN,
    } pull : 2;
};
