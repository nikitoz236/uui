#pragma once
#include <stdint.h>

// XL9555 base I2C address — A2:A1:A0 pins set offset
#define XL9555_BASE_ADDR  0x20

typedef enum {
    XL9555_DIR_OUT = 0,
    XL9555_DIR_IN  = 1,
} xl9555_dir_t;

typedef enum {
    XL9555_POL_NORMAL   = 0,
    XL9555_POL_INVERTED = 1,
} xl9555_pol_t;

typedef struct __attribute__((packed)) {
    uint8_t addr_offset : 3;  // A2:A1:A0 pins state
    uint8_t pin         : 4;  // GPIO pin number 0–15
    uint8_t dir         : 1;  // xl9555_dir_t
    uint8_t polarity    : 1;  // xl9555_pol_t
} xl9555_gpio_t;

void init_xl9555_gpio(const xl9555_gpio_t * gpio);
void xl9555_gpio_set(const xl9555_gpio_t * gpio, unsigned state);
unsigned xl9555_gpio_get(const xl9555_gpio_t * gpio);
