#include "lcd_spi.h"
#include "lcd_fb.h"
#include "delay_blocking.h"

extern const fb_desc_t __lcd_fb_desc;

/*
    Sitronix ST7567 — monochrome COG LCD controller (до 132 x 65)

    https://www.crystalfontz.com/controllers/Sitronix/ST7567/

    интерфейс: 4-wire SPI (8 bit) + отдельный D/C (RS)
*/

#define LCD_CMD_RESET                   0xE2

#define LCD_CMD_DISPLAY_ON              0xAF
#define LCD_CMD_DISPLAY_OFF             0xAE

#define LCD_CMD_ALL_ON                  0xA5    // все пиксели on (тест)
#define LCD_CMD_NORMAL                  0xA4    // вывод из RAM
#define LCD_CMD_INVERSE                 0xA7
#define LCD_CMD_NORMAL_DISP             0xA6

#define LCD_CMD_BIAS_1_9                0xA2
#define LCD_CMD_BIAS_1_7                0xA3

#define LCD_CMD_SEG_NORMAL              0xA0
#define LCD_CMD_SEG_REVERSE             0xA1
#define LCD_CMD_COM_NORMAL              0xC0
#define LCD_CMD_COM_REVERSE             0xC8

#define LCD_CMD_POWER(b)                (0x28 | ((b) & 0x07))   // bit0=follower, bit1=regulator, bit2=booster
#define LCD_CMD_REG_RATIO(r)            (0x20 | ((r) & 0x07))   // V0 regulation ratio 0..7
#define LCD_CMD_CONTRAST                0x81                    // далее байт 0..63
#define LCD_CMD_BOOSTER_RATIO           0xF8                    // далее 0x00=4x, 0x01=5x, 0x03=6x

#define LCD_CMD_START_LINE(l)           (0x40 | ((l) & 0x3F))
#define LCD_CMD_SET_PAGE(p)             (0xB0 | ((p) & 0x0F))
#define LCD_CMD_SET_COL_HI(c)           (0x10 | (((c) >> 4) & 0x0F))
#define LCD_CMD_SET_COL_LO(c)           (0x00 | ((c)       & 0x0F))

static const lcd_cfg_t * lcd_cfg;

static inline void lcd_send_cmd(uint8_t cmd)
{
    gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 0);
    spi_write_8(lcd_cfg->spi_dev.spi, cmd);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
}

static inline void lcd_send_data(uint8_t data)
{
    gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 1);
    spi_write_8(lcd_cfg->spi_dev.spi, data);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
}

void init_lcd(const lcd_cfg_t * cfg)
{
    lcd_cfg = cfg;

    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 0);
    delay_ms(10);
    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 1);
    delay_ms(10);

    lcd_select();

    lcd_send_cmd(LCD_CMD_RESET);
    delay_ms(10);

    lcd_send_cmd(LCD_CMD_BIAS_1_9);
    // природная ориентация COG-модуля (FPC снизу) = SEG_REVERSE + COM_NORMAL,
    // флаги x_flip / y_flip отсчитываются от неё
    if (cfg->gcfg.x_flip) {
        lcd_send_cmd(LCD_CMD_SEG_NORMAL);
    } else {
        lcd_send_cmd(LCD_CMD_SEG_REVERSE);
    }
    if (cfg->gcfg.y_flip) {
        lcd_send_cmd(LCD_CMD_COM_REVERSE);
    } else {
        lcd_send_cmd(LCD_CMD_COM_NORMAL);
    }
    lcd_send_cmd(LCD_CMD_START_LINE(0));

    // booster / regulator / follower включаются поэтапно с паузами
    lcd_send_cmd(LCD_CMD_POWER(0x4));       // booster on
    delay_ms(50);
    lcd_send_cmd(LCD_CMD_POWER(0x6));       // booster + regulator
    delay_ms(50);
    lcd_send_cmd(LCD_CMD_POWER(0x7));       // booster + regulator + follower
    delay_ms(10);

    // booster ratio = 5x — даёт V_LCD достаточный для transmissive 128x64
    lcd_send_cmd(LCD_CMD_BOOSTER_RATIO);
    lcd_send_cmd(0x01);

    lcd_send_cmd(LCD_CMD_REG_RATIO(4));

    lcd_send_cmd(LCD_CMD_CONTRAST);
    lcd_send_cmd(28);                       // EV 0..63

    lcd_send_cmd(LCD_CMD_NORMAL);
    lcd_send_cmd(LCD_CMD_NORMAL_DISP);
    lcd_send_cmd(LCD_CMD_DISPLAY_ON);

    lcd_unselect();
}

void lcd_pwr(unsigned val)
{
    lcd_select();
    if (val) {
        lcd_send_cmd(LCD_CMD_POWER(7));
        lcd_send_cmd(LCD_CMD_DISPLAY_ON);
    } else {
        lcd_send_cmd(LCD_CMD_DISPLAY_OFF);
        lcd_send_cmd(LCD_CMD_POWER(0));
    }
    lcd_unselect();
}

void lcd_refresh(void)
{
    const fb_desc_t * d = &__lcd_fb_desc;

    // RAM контроллера 132 колонки, видимая ширина модуля d->w.
    // при SEG_REVERSE (x_flip=0) колонка 0 RAM маппится в правый край стекла,
    // поэтому стартовать надо со смещением 132 - d->w
    unsigned col_start;
    if (lcd_cfg->gcfg.x_flip) {
        col_start = 0;
    } else {
        col_start = 132 - d->w;
    }

    lcd_select();

    for (unsigned page = 0; page < d->rows; page++) {
        lcd_send_cmd(LCD_CMD_SET_PAGE(page));
        lcd_send_cmd(LCD_CMD_SET_COL_HI(col_start));
        lcd_send_cmd(LCD_CMD_SET_COL_LO(col_start));

        gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 1);
        const uint8_t * row = &d->fb_ctx->fb[page * d->w];
        for (coord_t col = 0; col < d->w; col++) {
            spi_write_8(lcd_cfg->spi_dev.spi, row[col]);
            while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        }
    }

    lcd_unselect();
}

