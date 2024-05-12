#include "gpio.h"
#include <stdint.h>

struct gpio_pin {
    uint8_t port : 4;
    uint8_t pin : 4;
};

struct gpio_cfg {
    enum {
        GPIO_MODE_ANALOG,
        GPIO_MODE_INPUT,
        GPIO_MODE_OUTPUT,
        GPIO_MODE_AF,
    } mode;
    enum {
        GPIO_SPEED_LOW = 0b10,
        GPIO_SPEED_MED = 0b01,
        GPIO_SPEED_HIGH = 0b11,
    } speed;
    enum {
        GPIO_TYPE_PP,
        GPIO_TYPE_OD,
    } type;
    enum {
        GPIO_PULL_NONE,
        GPIO_PULL_UP,
        GPIO_PULL_DOWN,
    } pull;
};
