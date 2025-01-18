#include "stm32f0x_gpio.h"
#include "bit_fields.h"

static GPIO_TypeDef * gpio_ports[] = {
    [GPIO_PORT_A] = GPIOA,
    [GPIO_PORT_B] = GPIOB,
    [GPIO_PORT_C] = GPIOC,
    [GPIO_PORT_D] = GPIOD,
    [GPIO_PORT_E] = GPIOE,
    [GPIO_PORT_F] = GPIOF,
};

void bf_write(volatile void * array, unsigned index, unsigned field_len, unsigned value)
{
    bf_set((void *)array, index, field_len, value);
}

void gpio_set_cfg(const gpio_pin_t * pin, const gpio_cfg_t * cfg)
{
    GPIO_TypeDef * port = gpio_ports[pin->port];
    port->MODER &= ~(3 << (pin->pin * 2));
    port->MODER |= cfg->mode << (pin->pin * 2);

    port->OSPEEDR &= ~(3 << (pin->pin * 2));
    port->OSPEEDR |= cfg->speed << (pin->pin * 2);

    port->PUPDR &= ~(3 << (pin->pin * 2));
    port->PUPDR |= cfg->pull << (pin->pin * 2);

    port->OTYPER &= ~(1 << pin->pin);
    port->OTYPER |= cfg->type << pin->pin;

    if (pin->pin < 8) {
        port->AFR[0] &= ~(0xF << (pin->pin * 4));
        port->AFR[0] |= cfg->af << (pin->pin * 4);
    } else {
        port->AFR[1] &= ~(0xF << ((pin->pin - 8) * 4));
        port->AFR[1] |= cfg->af << ((pin->pin - 8) * 4);
    }

    // bf_set(&port->OSPEEDR, pin->pin, 2, cfg->speed);
    // bf_set(&port->OTYPER, pin->pin, 1, cfg->type);
    // bf_set(&port->PUPDR, pin->pin, 2, cfg->pull);
    // bf_set(&port->AFR[0], pin->pin, 4, cfg->af);
}

void gpio_set_state(const gpio_pin_t * pin, unsigned state)
{
    GPIO_TypeDef * port = gpio_ports[pin->port];
    if (state) {
        port->BSRR = 1 << pin->pin;
    } else {
        port->BRR = 1 << pin->pin;
    }
}

unsigned gpio_get_state(const gpio_pin_t * pin)
{
    GPIO_TypeDef * port = gpio_ports[pin->port];
    if (port->IDR & (1 << pin->pin)) {
        return 1;
    }
    return 0;
}

void gpio_configure(const gpio_pin_cfg_t * pin_cfg)
{
    gpio_set_cfg(&pin_cfg->gpio, &pin_cfg->cfg);
}
