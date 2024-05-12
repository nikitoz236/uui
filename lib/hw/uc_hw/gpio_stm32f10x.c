#include "gpio_stm32f10x.h"
#include "stm32f10x.h"

static GPIO_TypeDef * gpio_ports[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
};

void gpio_set_cfg(gpio_pin_t * pin, gpio_cfg_t * cfg)
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

    GPIO_TypeDef * port = gpio_ports[pin->port];
    unsigned pin_mode_shift = pin->pin * 4;
    __IO uint32_t * ctrl_reg;
    if (pin->pin < 8) {
        ctrl_reg = &port->CRL;
    } else {
        ctrl_reg = &port->CRH;
        pin_mode_shift -= 8 * 4;
    }

    *ctrl_reg &= ~(0xF << pin_mode_shift);
    *ctrl_reg |= cr.value << pin_mode_shift;
    if (odr_set) {
        port->ODR |= 1 << pin->pin;
    }
}

void gpio_set_state(gpio_pin_t * pin, unsigned state)
{
    GPIO_TypeDef * port = gpio_ports[pin->port];
    if (state) {
        port->BSRR = 1 << pin->pin;
    } else {
        port->BRR = 1 << pin->pin;
    }
}

unsigned gpio_get_state(gpio_pin_t * pin)
{
    GPIO_TypeDef * port = gpio_ports[pin->port];
    if (port->IDR & (1 << pin->pin)) {
        return 1;
    }
    return 0;
}
