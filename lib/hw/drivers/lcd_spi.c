#include "lcd_spi.h"

const lcd_cfg_t * lcd_hw_cfg;

void init_lcd_hw(const lcd_cfg_t * cfg)
{
    lcd_hw_cfg = cfg;
    init_spi_dev(&cfg->spi_dev);
    init_gpio_list(cfg->ctrl_lines);
    init_spi(cfg->spi_dev.spi);
    if (cfg->bl) {
        init_pwm(cfg->bl);
    }
}

void lcd_bl(unsigned val)
{
    pwm_set_ccr(lcd_hw_cfg->bl, val);
}
