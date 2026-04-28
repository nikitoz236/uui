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

#define CMD_CONTRAST           0x81 // Set Contrast Control
#define CMD_PRECHARGE          0xD9 // Set Pre-charge Period
#define CMD_VCOMH              0xDB // Set VCOMH Deselect Level

#define CMD_DISPLAY_OFF        0xAE
#define CMD_DISPLAY_ON         0xAF
#define CMD_CLOCK_DIV          0xD5 // Set Display Clock Divide Ratio / Oscillator Frequency
#define CMD_MULTIPLEX          0xA8 // Set Multiplex Ratio
#define CMD_DISPLAY_OFFSET     0xD3 // Set Display Offset
#define CMD_START_LINE         0x40 // Set Display Start Line (нижние 6 бит — line, у нас 0)
#define CMD_CHARGE_PUMP        0x8D // Charge Pump Setting
#define CMD_ADDR_MODE          0x20 // Set Memory Addressing Mode
#define CMD_SEG_REMAP_REVERSED 0xA1 // Segment Re-map: col 127 -> SEG0 (норма 0xA0)
#define CMD_COM_SCAN_REVERSED  0xC8 // COM Output Scan Direction: reversed (норма 0xC0)
#define CMD_COM_PINS           0xDA // Set COM Pins Hardware Configuration
#define CMD_RESUME_RAM         0xA4 // Entire Display ON: resume from RAM (0xA5 — все пиксели on)
#define CMD_NORMAL_DISPLAY     0xA6 // Set Normal Display (0xA7 — inverted)
#define CMD_DEACTIVATE_SCROLL  0x2E

#define CMD_COL_ADDR           0x21 // Set Column Address
#define CMD_PAGE_ADDR          0x22 // Set Page Address

typedef uint8_t ssd1306_contrast_t;

typedef union {
    uint8_t raw;
    struct {
        uint8_t phase1 : 4;         // [3:0] phase1 — длительность фазы 1 в DCLK, 1..15 (0 invalid, reset 2)
        uint8_t phase2 : 4;         // [7:4] phase2 — длительность фазы 2 в DCLK, 1..15 (0 invalid, reset 2)
    };
} ssd1306_precharge_t;

typedef union {
    uint8_t raw;
    struct {
        uint8_t : 4;
        enum {
            VCOMH_065 = 0,          // 0.65 * Vcc
            VCOMH_077 = 2,          // 0.77 * Vcc
            VCOMH_083 = 3,          // 0.83 * Vcc
            VCOMH_MAX = 4,          // выше datasheet
        } level : 3;
        uint8_t : 1;
    };
} ssd1306_vcomh_t;

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
    CMD_DISPLAY_OFF,            0,
    CMD_CLOCK_DIV,              1, 0x80, // freq=8 (default), div=0
    CMD_MULTIPLEX,              1, 0x3F, // 64-1
    CMD_DISPLAY_OFFSET,         1, 0x00,
    CMD_START_LINE,             0,
    CMD_CHARGE_PUMP,            1, 0x14, // internal DC-DC
    CMD_ADDR_MODE,              1, 0x00, // horizontal
    CMD_SEG_REMAP_REVERSED,     0,
    CMD_COM_SCAN_REVERSED,      0,
    CMD_COM_PINS,               1, 0x12, // alt config, 64 rows
    CMD_PRECHARGE,              1, 0xF1, // phase1=1, phase2=15
    CMD_RESUME_RAM,             0,
    CMD_NORMAL_DISPLAY,         0,
    CMD_DEACTIVATE_SCROLL,      0,
};

static const struct {
    ssd1306_contrast_t contrast;
    ssd1306_vcomh_t vcomh;
} br_table[] = {
    { 0x00, { .level = VCOMH_065 } },
    { 0x10, { .level = VCOMH_065 } },
    { 0x20, { .level = VCOMH_065 } },
    { 0x10, { .level = VCOMH_077 } },
    { 0x40, { .level = VCOMH_077 } },
    { 0x60, { .level = VCOMH_077 } },
    { 0x80, { .level = VCOMH_083 } },
    { 0x80, { .level = VCOMH_MAX } },
    { 0xCF, { .level = VCOMH_MAX } },
};

static void send_brightness(unsigned lvl)
{
    if (lvl >= ARRAY_SIZE(br_table)) {
        lvl = ARRAY_SIZE(br_table) - 1;
    }
    uint8_t buf[] = {
        CMD_CONTRAST, 1, br_table[lvl].contrast,
        CMD_VCOMH, 1, br_table[lvl].vcomh.raw,
    };
    lcd_select();
    unsigned idx = 0;
    while (idx < sizeof(buf)) {
        const lcd_cmd_t * cmd = (const lcd_cmd_t *) &buf[idx];
        lcd_send_cmd_with_data(cmd);
        idx += sizeof(lcd_cmd_t) + cmd->len;
    }
    lcd_unselect();
}

void lcd_pwr(unsigned val)
{
    lcd_select();
    if (val) {
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = CMD_DISPLAY_ON });
    } else {
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = CMD_DISPLAY_OFF });
    }
    lcd_unselect();
}

void lcd_refresh(void)
{
    const fb_desc_t * d = &__lcd_fb_desc;
    static const uint8_t set_area[] = {
        CMD_COL_ADDR, 2, 0, 127,
        CMD_PAGE_ADDR, 2, 0, 7,
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
    (void)cfg;
}

void bl_set(backlight_cfg_t * cfg, unsigned lvl)
{
    (void) cfg;
    send_brightness(lvl);
}
