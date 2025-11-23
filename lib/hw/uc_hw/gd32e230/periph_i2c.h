#pragma once

#include "i2c.h"
#include "gpio.h"
#include "pclk.h"
#include "periph_header.h"

struct i2c_cfg {
    gpio_list_t * pins;
    I2C_TypeDef * i2c;
    uint8_t irqn;
    uint8_t err_irqn;
    pclk_t pclk;
    unsigned freq;
};
