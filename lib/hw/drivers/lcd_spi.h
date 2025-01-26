#pragma once
#include "gpio.h"
#include "spi.h"
#include "pwm.h"

// больше вероятность что не будет DC, черм RESET

enum {
    LCD_RST,
    LCD_DC,
};

struct lcd_cfg {
    pwm_cfg_t * bl;
    gpio_list_t * ctrl_lines;
    spi_dev_cfg_t spi_dev;
};

typedef const struct lcd_cfg lcd_cfg_t;

void init_lcd_hw(const lcd_cfg_t * cfg);
void init_lcd(const lcd_cfg_t * cfg);
void lcd_pwr(unsigned val);
void lcd_bl(unsigned val);
