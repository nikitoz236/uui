#pragma once
#include "gpio.h"
#include "spi.h"
#include "pwm.h"

// больше вероятность что не будет DC, черм RESET

enum {
    LCD_RST,
    LCD_DC,
};

struct lcd_glass_cfg {
    uint16_t width;
    uint16_t height;
    uint16_t x_offset;
    uint16_t y_offset;
    uint8_t x_flip : 1;
    uint8_t y_flip : 1;
    uint8_t xy_swap : 1;
};

struct lcd_cfg {
    pwm_cfg_t * bl;
    gpio_list_t * ctrl_lines;
    spi_dev_cfg_t spi_dev;
    struct lcd_glass_cfg gcfg;
};

typedef const struct lcd_cfg lcd_cfg_t;

void init_lcd_hw(const lcd_cfg_t * cfg);
void init_lcd(const lcd_cfg_t * cfg);
void lcd_pwr(unsigned val);
void lcd_bl(unsigned val);
