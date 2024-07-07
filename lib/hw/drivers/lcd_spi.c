#include "lcd_spi.h"

const lcd_cfg_t * lcd_hw_cfg;

void init_lcd_hw(const lcd_cfg_t * cfg)
{
    lcd_hw_cfg = cfg;
    if (cfg->dc.port != GPIO_EMPTY) {
        gpio_set_cfg(&cfg->dc, cfg->spi_slave.cs_pin_cfg);
        gpio_set_state(&cfg->dc, 0);
    }
    if (cfg->rst.port != GPIO_EMPTY) {
        gpio_set_cfg(&cfg->rst, cfg->spi_slave.cs_pin_cfg);
        gpio_set_state(&cfg->rst, 0);
    }
    if (cfg->spi_slave.cs_pin.port != GPIO_EMPTY) {
        gpio_set_cfg(&cfg->spi_slave.cs_pin, cfg->spi_slave.cs_pin_cfg);
        gpio_set_state(&cfg->spi_slave.cs_pin, 1);
    }

    init_spi(cfg->spi_slave.spi);
    init_pwm(cfg->bl);
}

void lcd_bl(unsigned val)
{
    pwm_set_ccr(lcd_hw_cfg->bl, val);
}
