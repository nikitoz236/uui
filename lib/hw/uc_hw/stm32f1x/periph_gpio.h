/*
    STM32F1 gpio inplementation
*/

#pragma once
#include "gpio.h"
#include "../stm_gpio.h"

typedef struct __attribute__((packed)) {
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
