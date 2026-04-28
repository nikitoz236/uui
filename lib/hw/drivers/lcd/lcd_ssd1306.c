#include "lcd_spi.h"
#include "lcd_fb.h"
#include "array_size.h"
#include "delay_blocking.h"

extern const fb_desc_t __lcd_fb_desc;

/*
    https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
*/

struct backlight_cfg {
    uint8_t initial;
};

typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t data[];
} lcd_cmd_t;

static const lcd_cfg_t * lcd_cfg;

static void lcd_send_cmd_with_data(const lcd_cmd_t * cmd)
{
    gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 0);
    spi_write_8(lcd_cfg->spi_dev.spi, cmd->cmd);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
    for (uint8_t i = 0; i < cmd->len; i++) {
        spi_write_8(lcd_cfg->spi_dev.spi, cmd->data[i]);
        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
    }
}

static const uint8_t lcd_init_cmd_list[] = {
    0xAE, 0,                    // Display OFF
    0xD5, 1,    0x80,           // Clock div / oscillator freq
    0xA8, 1,    0x3F,           // Multiplex ratio = 64-1
    0xD3, 1,    0x00,           // Display offset
    0x40, 0,                    // Start line = 0
    0x8D, 1,    0x14,           // Charge pump enable
    0x20, 1,    0x00,           // Horizontal addressing mode
    0xA1, 0,                    // Segment remap (col 127 -> SEG0)
    0xC8, 0,                    // COM scan reversed
    0xDA, 1,    0x12,           // COM pins config
    0x81, 1,    0xCF,           // Contrast
    0xD9, 1,    0xF1,           // Pre-charge
    0xDB, 1,    0x40,           // VCOMH deselect
    0xA4, 0,                    // Resume RAM content
    0xA6, 0,                    // Normal (not inverted)
    0x2E, 0,                    // Deactivate scroll
};

void lcd_pwr(unsigned val)
{
    lcd_select();
    lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = val ? 0xAF : 0xAE });
    lcd_unselect();
}

void lcd_refresh(void)
{
    const fb_desc_t * d = &__lcd_fb_desc;
    static const uint8_t set_area[] = {
        0x21, 2,    0, 127,
        0x22, 2,    0, 7,
    };

    lcd_select();

    unsigned idx = 0;
    while (idx < sizeof(set_area)) {
        const lcd_cmd_t * cmd = (const lcd_cmd_t *) &set_area[idx];
        lcd_send_cmd_with_data(cmd);
        idx += sizeof(lcd_cmd_t) + cmd->len;
    }

    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
    gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 1);
    for (lcd_fb_size_t i = 0; i < d->fb_len; i++) {
        spi_write_8(lcd_cfg->spi_dev.spi, d->fb_ctx->fb[i]);
        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
    }

    lcd_unselect();
}

void init_lcd(const lcd_cfg_t * cfg)
{
    lcd_cfg = cfg;

    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 0);
    delay_ms(10);
    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 1);
    delay_ms(10);

    lcd_select();
    unsigned idx = 0;
    while (idx < ARRAY_SIZE(lcd_init_cmd_list)) {
        const lcd_cmd_t * cmd = (const lcd_cmd_t *) &lcd_init_cmd_list[idx];
        lcd_send_cmd_with_data(cmd);
        idx += sizeof(lcd_cmd_t) + cmd->len;
    }
    lcd_unselect();

    lcd_pwr(1);
}

void init_backlight(backlight_cfg_t * cfg)
{
    (void) cfg;
}

void bl_set(backlight_cfg_t * cfg, unsigned lvl)
{
    (void) cfg;
    (void) lvl;
}
