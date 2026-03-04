#include "lcd_spi.h"

const lcd_cfg_t * lcd_hw_cfg;

void init_lcd_hw(const lcd_cfg_t * cfg)
{
    lcd_hw_cfg = cfg;
    init_spi_dev(&cfg->spi_dev);
    init_gpio_list(cfg->ctrl_lines);
    init_spi(cfg->spi_dev.spi);
    if (cfg->bl) {
        init_backlight(cfg->bl);
    }
}

void lcd_bl(unsigned val)
{
    bl_set(lcd_hw_cfg->bl, val);
}

void lcd_select(void)
{
    spi_set_frame_len(lcd_hw_cfg->spi_dev.spi, 8);
    spi_dev_select(&lcd_hw_cfg->spi_dev);
}

void lcd_unselect(void)
{
    spi_set_frame_len(lcd_hw_cfg->spi_dev.spi, 8);
    spi_dev_unselect(&lcd_hw_cfg->spi_dev);
}
