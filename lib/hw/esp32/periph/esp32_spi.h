#pragma once
#include "spi.h"
#include "gpio.h"

#include "soc/spi_struct.h"

struct spi_cfg {
    spi_dev_t * spi;
    gpio_list_t * gpio_list;
};
