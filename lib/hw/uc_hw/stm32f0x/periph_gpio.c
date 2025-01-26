/*
    STM32F0 gpio inplementation
*/

#include "periph_gpio.h"
// #include "bit_fields.h"

static void gpio_set_cfg(gpio_pin_t pin, gpio_cfg_t cfg)
{
    GPIO_TypeDef * port = __gpio_port(pin);
    port->MODER &= ~(3 << (pin.pin * 2));
    port->MODER |= cfg.mode << (pin.pin * 2);

    port->OSPEEDR &= ~(3 << (pin.pin * 2));
    port->OSPEEDR |= cfg.speed << (pin.pin * 2);

    port->PUPDR &= ~(3 << (pin.pin * 2));
    port->PUPDR |= cfg.pull << (pin.pin * 2);

    port->OTYPER &= ~(1 << pin.pin);
    port->OTYPER |= cfg.type << pin.pin;

    if (pin.pin < 8) {
        port->AFR[0] &= ~(0xF << (pin.pin * 4));
        port->AFR[0] |= cfg.af << (pin.pin * 4);
    } else {
        port->AFR[1] &= ~(0xF << ((pin.pin - 8) * 4));
        port->AFR[1] |= cfg.af << ((pin.pin - 8) * 4);
    }

    // bf_set(&port->OSPEEDR, pin.pin, 2, cfg.speed);
    // bf_set(&port->OTYPER, pin.pin, 1, cfg.type);
    // bf_set(&port->PUPDR, pin.pin, 2, cfg.pull);
    // bf_set(&port->AFR[0], pin.pin, 4, cfg.af);
}

void init_gpio(const gpio_t * gpio)
{
    if (gpio) {
        gpio_set_cfg(gpio->gpio, gpio->cfg);
    }
}

void gpio_set_state(const gpio_t * gpio, unsigned state)
{
    stm_gpio_set_state(gpio->gpio, state);
}

unsigned gpio_get_state(const gpio_t * gpio)
{
    return stm_gpio_get_state(gpio->gpio);
}

void init_gpio_list(const gpio_list_t * gpio_list)
{
    for (unsigned i = 0; i < gpio_list->count; i++) {
        gpio_set_cfg(gpio_list->pin_list[i], gpio_list->cfg);
    }
}

void gpio_list_set_state(const gpio_list_t * gpio_list, unsigned idx, unsigned state)
{
    stm_gpio_set_state(gpio_list->pin_list[idx], state);
}

unsigned gpio_list_get_state(const gpio_list_t * gpio_list, unsigned idx)
{
    return stm_gpio_get_state(gpio_list->pin_list[idx]);
}
