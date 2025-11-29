#pragma once
#include "spi.h"
#include "gpio.h"

#include "soc/spi_struct.h"

struct spi_cfg {
    spi_dev_t * spi;
    gpio_list_t * gpio_list;
};


void init_spi(const spi_cfg_t * cfg);
unsigned spi_is_busy(const spi_cfg_t * cfg);
void spi_set_frame_len(const spi_cfg_t * cfg, unsigned len);
