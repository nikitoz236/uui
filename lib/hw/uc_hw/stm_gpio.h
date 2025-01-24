#pragma once
#include <stdint.h>
#include "periph_header.h"

enum {
    GPIO_EMPTY = 0,
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
};

typedef struct __attribute__((packed)) {
    uint8_t port;
    uint8_t pin;
} gpio_pin_t;

static inline GPIO_TypeDef * __gpio_port(gpio_pin_t gpio)
{
    if (gpio.port == GPIO_EMPTY) {
        return 0;
    }
    const unsigned step = GPIOB_BASE - GPIOA_BASE;
    return (GPIO_TypeDef *)(GPIOA_BASE + ((gpio.port - 1) * step));
}

static inline void stm_gpio_set_state(gpio_pin_t gpio, unsigned state)
{
    GPIO_TypeDef * port = __gpio_port(gpio);
    if (port) {
        if (state) {
            port->BSRR = 1 << gpio.pin;
        } else {
            port->BRR = 1 << gpio.pin;
        }
    }
}

static inline unsigned stm_gpio_get_state(gpio_pin_t gpio)
{
    GPIO_TypeDef * port = __gpio_port(gpio);
    // оператор && выполняется слева направо
    if (port && (port->IDR & (1 << gpio.pin))) {
        return 1;
    }
    return 0;
}

/*
    в чем собственно проблема ?
    я не могу препроцессором инициализировать целое число как структуру засунуть в структуру с конфигом 

    бывает 2 сценария
        дофига одинаковых выходов, прям массив пинов с одинаковым кофигом
        ноги которые чаще AF, где у каждой ноги свой конфиг.

    ВАРИАНТЫ
        - общая структура для всех стмок
        - передавать указатели
        - инициализировать через жопу, сдвигами


    варианты
        хочется чтобы в случаях когда нога должна 



*/

