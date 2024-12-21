#pragma once
#include <stdint.h>

enum gpio_port {
    GPIO_EMPTY = 0,
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
};

// TODO check sizeof
struct gpio_pin {
    enum gpio_port port : 4;
    uint8_t pin : 4;
};

typedef struct gpio_pin gpio_pin_t;

struct gpio_cfg;
typedef struct gpio_cfg gpio_cfg_t;

void gpio_set_cfg(const gpio_pin_t * pin, const gpio_cfg_t * cfg);

typedef struct {
    gpio_pin_t gpio;
    gpio_cfg_t * cfg;
} gpio_pin_cfg_t;

static inline void gpio_configure(const gpio_pin_cfg_t * pin_cfg)
{
    gpio_set_cfg(&pin_cfg->gpio, pin_cfg->cfg);
}

void gpio_set_state(const gpio_pin_t * pin, unsigned state);
unsigned gpio_get_state(const gpio_pin_t * pin);
