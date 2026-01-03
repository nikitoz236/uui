#pragma once
#include "stdint.h"

#include "soc/i2c_struct.h"


#include "i2c.h"
#include "esp32_gpio.h"
#include "esp32_pclk.h"

struct i2c_cfg {
    i2c_dev_t * dev;
    gpio_list_t * pin_list;
    uint32_t freq;
    pclk_t i2c_pclk;
};
