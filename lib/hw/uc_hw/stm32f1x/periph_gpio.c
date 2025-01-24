#include "periph_gpio.h"

static void gpio_set_cfg(gpio_pin_t pin, const gpio_cfg_t * cfg)
{
    union cr_field {
        struct {
            unsigned mode : 2;
            unsigned cnf : 2;
        };
        unsigned value;
    };

    union cr_field cr = {};
    unsigned odr_set = 0;

    if (cfg->mode >= GPIO_MODE_OUTPUT) {
        // OUTPUT
        cr.mode = cfg->speed;
        if (cfg->type == GPIO_TYPE_OD) {
            cr.cnf |= 1;
        }
        if (cfg->mode == GPIO_MODE_AF) {
            cr.cnf |= 2;
        }
    } else {
        if (cfg->mode == GPIO_MODE_INPUT) {
            if (cfg->pull == GPIO_PULL_NONE) {
                cr.cnf |= 1;
            } else {
                cr.cnf |= 2;
                if (cfg->pull == GPIO_PULL_UP) {
                    odr_set = 1;
                }
            }
        }
    }

    GPIO_TypeDef * port = __gpio_port(pin);
    unsigned pin_mode_shift = pin.pin * 4;
    __IO uint32_t * ctrl_reg;
    if (pin.pin < 8) {
        ctrl_reg = &port->CRL;
    } else {
        ctrl_reg = &port->CRH;
        pin_mode_shift -= 8 * 4;
    }

    *ctrl_reg &= ~(0xF << pin_mode_shift);
    *ctrl_reg |= cr.value << pin_mode_shift;
    if (odr_set) {
        port->ODR |= 1 << pin.pin;
    }
}

void init_gpio(const gpio_t * gpio)
{
    gpio_set_cfg(gpio->gpio, &gpio->cfg);
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
        gpio_set_cfg(gpio_list->pin_list[i], &gpio_list->cfg);
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
