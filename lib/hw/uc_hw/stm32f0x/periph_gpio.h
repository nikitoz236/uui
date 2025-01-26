/*
    STM32F0 gpio inplementation
*/

#pragma once
#include "gpio.h"
#include "../stm_gpio.h"

typedef struct {
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
} gpio_cfg_t;

struct __attribute__((packed)) gpio {
    gpio_pin_t gpio;
    gpio_cfg_t cfg;
};

struct gpio_list {
    gpio_pin_t * pin_list;
    gpio_cfg_t cfg;
    uint8_t count;
};
