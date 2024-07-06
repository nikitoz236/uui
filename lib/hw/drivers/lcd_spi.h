#pragma once
#include "gpio.h"
#include "spi.h"
#include "pwm.h"

struct lcd_cfg {
    spi_slave_cfg_t spi_slave;
    gpio_pin_t dc;
    gpio_pin_t rst;
    pwm_cfg_t bl;
};

typedef struct lcd_cfg lcd_cfg_t;

void init_lcd_hw(const lcd_cfg_t * cfg);
void init_lcd(const lcd_cfg_t * cfg);
void lcd_pwr(unsigned val);
void lcd_bl(unsigned val);
