#include "lcd_spi.h"
#include "lcd_fb.h"
#include "delay_blocking.h"

extern const fb_desc_t __lcd_fb_desc;

/*

http://uglyduck.vajn.icu/PDF/lcd/Nokia1202/STE2007.pdf
http://mylcd.sourceforge.net/files/pcf8814.pdf

*/

#define LCD_CMD_RESET                   0xE2
#define LCD_CMD_DISPLAY_ON              0xAF
#define LCD_CMD_DISPLAY_OFF             0xAD

#define LCD_CMD_SET_PAGE(p)             (0xB0 + (p & 0xF))              // Page address set ( 1011XXXX )
#define LCD_CMD_SET_COLUMN_UPPER(c)     (0x10 + ((c >> 4) & 0x7))       // Column address set Upper bit address ( 0001 0HHH )
#define LCD_CMD_SET_COLUMN_LOWER(c)     (0x00 + (c & 0xF))              // Column address set Lower bit address ( 0000 LLLL )

#define LCD_CMD_MIRROR_Y(my)            (0xC0 + (my << 3))
#define LCD_CMD_MIRROR_X(mx)            (0xA0 + (mx))

static const lcd_cfg_t * lcd_cfg;

static inline void lcd_send_cmd(uint8_t cmd)
{
    spi_write_16(lcd_cfg->spi_dev.spi, cmd);
}

static inline void lcd_send_data(uint8_t data)
{
    spi_write_16(lcd_cfg->spi_dev.spi, data + (1 << 8));
}

void init_lcd(const lcd_cfg_t * cfg)
{
    lcd_cfg = cfg;

    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 0);
    delay_ms(3);
    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 1);
    delay_ms(3);

    lcd_select();
    spi_set_frame_len(cfg->spi_dev.spi, 9);

    lcd_send_cmd(LCD_CMD_RESET);

    lcd_send_cmd(0x3D);                 // Charge pump
    lcd_send_cmd(0x01);                 // Charge pump = 4 (default 5 is too hight for 3.0 volt)

    lcd_send_cmd(0xA6);                 // Normal:DDRAM Data ”H”=LCD ON voltage

    lcd_send_cmd(0xA4);                 // Normal Display Mode
    lcd_send_cmd(0x80 + 16);            // Electronic volume ( 100X XXXX )

    lcd_send_cmd(0x40);                 // Display start line address set 0

    lcd_pwr(1);

    // lcd_clear();
    // lcd_refresh();

    // lcd_unselect();
}

void lcd_pwr(unsigned val)
{
    lcd_select();
    spi_set_frame_len(lcd_cfg->spi_dev.spi, 9);
    if (val) {
        // ~ 0.25 mA @ 3.3V
        lcd_send_cmd(0x2F);     // Power control set: Booster : ON Voltage regulator : ON Voltage follower : ON
        lcd_send_cmd(LCD_CMD_DISPLAY_ON);
    } else {
        // ~ 0.05 mA @ 3.3V
        lcd_send_cmd(0x28);     // Power control set: Booster : OFF Voltage regulator : OFF Voltage follower : OFF
        lcd_send_cmd(LCD_CMD_DISPLAY_OFF);
    }
    lcd_unselect();
}

void lcd_refresh(void)
{
    const fb_desc_t * d = &__lcd_fb_desc;

    lcd_select();
    spi_set_frame_len(lcd_cfg->spi_dev.spi, 9);

    lcd_send_cmd(LCD_CMD_SET_COLUMN_UPPER(0));
    lcd_send_cmd(LCD_CMD_SET_COLUMN_LOWER(0));
    lcd_send_cmd(LCD_CMD_SET_PAGE(0));

    for (lcd_fb_size_t i = 0; i < d->fb_len; i++) {
        lcd_send_data(d->fb_ctx->fb[i]);
    }

    lcd_unselect();
}
