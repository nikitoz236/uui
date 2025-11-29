#pragma once
#include "gpio.h"
#include <stdint.h>

#include "soc/gpio_sig_map.h"
#include "flex_wrap.h"

typedef enum {
    GPIO_MODE_IN,
    GPIO_MODE_OUT,
    GPIO_MODE_MUX,
    GPIO_MODE_SIG_IN,
    GPIO_MODE_SIG_OUT,
    GPIO_MODE_SIG_IO,
} gpio_mode_t;

typedef struct __attribute__((packed)) {
    gpio_mode_t mode : 3;
    uint8_t pu : 1;
    uint8_t pd : 1;
    uint8_t od : 1;
} gpio_cfg_t;

typedef struct __attribute__((packed)) {
    uint8_t signal;
    uint8_t pin;
} gpio_pin_t;

struct __attribute__((packed)) gpio {
    gpio_cfg_t cfg;
    gpio_pin_t pin;
};

struct gpio_list {
    gpio_cfg_t cfg;
    uint8_t count;
    gpio_pin_t pin_list[];
};
