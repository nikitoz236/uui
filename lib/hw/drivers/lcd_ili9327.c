#include "lcd_spi.h"
#include "array_size.h"
#include "api_lcd_color.h"
#include "delay_blocking.h"
#include "buf_endian.h"

static const lcd_cfg_t * lcd_cfg;

typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t data[];
} lcd_cmd_t;

void lcd_send_cmd_with_data(const lcd_cmd_t * cmd)
{
    // cmd
    spi_set_frame_len(lcd_cfg->spi_dev.spi, 8);
    gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 0);
    spi_write_8(lcd_cfg->spi_dev.spi, cmd->cmd);

    if (cmd->len == 0) {
        return;
    }

    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
    gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 1);
    // spi_dma_tx_buf(lcd_cfg->spi_dev.spi, cmd->data, cmd->len);

    for (unsigned i = 0; i < cmd->len; i++) {
        spi_write_8(lcd_cfg->spi_dev.spi, cmd->data[i]);
    }
}

const uint8_t lcd_init_cmd_list[] = {
//  cmd   len       data

    // Set EQ
    0xF3, 4,        0x08, 0x20, 0x20, 0x08,

    // OPON
    0xE7, 1,        0x60,

    // Set VCOM
    0xD1, 1,        0x00,0x5D,0x15,

    // Set power related setting
    0xD0, 5,        0x07,
                    0x02,                   //VGH:15V,VGL:-7.16V (BOE LCD: VGH:12~18V,VGL:-6~10V)
                    0x8B, 0x03, 0xD4,

    // Set_pixel_format
    0x3A, 1,        0x55,              //0x55:16bit/pixel,65k;0x66:18bit/pixel,262k;

    // Set_address_mode
    0x36, 1,        0x38,

    // Exit_invert_mode
    0x20, 0,

    // Set Normal/Partial mode display timing
    0xC1, 4, 0x10, 0x1A, 0x02, 0x02,

    // Set display related setting
    0xC0, 6, 0x10, 0x31, 0x00, 0x00, 0x01, 0x02,

    // Set waveform timing
    0xC4, 1, 0x01,

    // Set oscillator
    0xC5, 2,        0x04, 0x01,        //72Hz

    // Set power for normal mode
    0xD2, 2,        0x01, 0x44,

    // Set gamma
    0xC8, 15,       0x00, 0x77, 0x77, 0x00, 0x04, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,

    // Set DGC LUT
    0xCA, 1,        0x00,

    // 3-Gamma Function Control
    0xEA, 1,        0x80,              // enable
};

void lcd_pwr(unsigned val)
{
    if (val) {
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x11}); // Sleep Out
        // данное время всетаки влияет, 10 мс мало. на экране картинка рассыпается. было 120.
        delay_ms(150);
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x29}); // Display on
    } else {
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x10}); // Enter Sleep Mode
        delay_ms(5);
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x28}); // Display off
    }
}

static void lcd_set_area(unsigned x, unsigned y, unsigned w, unsigned h)
{
    static struct {
        lcd_cmd_t cmd;
        uint8_t start[2];
        uint8_t end[2];
    } tmp;

    tmp.cmd.len = 4;

    // TODO, костыли потомучто ты не хочешь тратить время на переключение SPI ?
    u16_to_be_buf8(tmp.start, x);
    u16_to_be_buf8(tmp.end, x + w - 1);
    tmp.cmd.cmd = 0x2B;

    lcd_send_cmd_with_data(&tmp.cmd);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};

    u16_to_be_buf8(tmp.start, y);
    u16_to_be_buf8(tmp.end, y + h - 1);
    tmp.cmd.cmd = 0x2A;

    lcd_send_cmd_with_data(&tmp.cmd);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
}

#define __DBGPIO_LCD(n, x)

#define __DBGPIO_LCD_RECT(x)            __DBGPIO_LCD(0, x)
#define __DBGPIO_LCD_DMA_SEND(x)        __DBGPIO_LCD(1, x)
#define __DBGPIO_LCD_IMAGE(x)           __DBGPIO_LCD(0, x)

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, lcd_color_t color)
{
    __DBGPIO_LCD_RECT(1);
    static unsigned color_static;
    lcd_set_area(x, y, w, h);
    lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x2C, .len = 0 });
    spi_set_frame_len(lcd_cfg->spi_dev.spi, 16);
    gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 1);
    unsigned len = w * h;
    color_static = color;
    while (len) {
        unsigned chunk = len;
        if (chunk > UINT16_MAX) {
            chunk %= UINT16_MAX;
            if (chunk == 0) {
                chunk = UINT16_MAX;
            }
        }
        __DBGPIO_LCD_DMA_SEND(1);
        spi_dma_tx_repeat(lcd_cfg->spi_dev.spi, &color_static, chunk);
        __DBGPIO_LCD_DMA_SEND(0);
        len -= chunk;
    }
    __DBGPIO_LCD_RECT(0);
}

void lcd_image(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scale, lcd_color_t * buf)
{
    for (unsigned i = 0; i < h; i++) {
        for (unsigned j = 0; j < w; j++) {
            lcd_rect(x + j * scale, y + i * scale, scale, scale, buf[i * w + j]);
        }
    }
}

static inline void lcd_clear(void)
{
    const lcd_color_t clear_color = 0xACEF;
    lcd_rect(0, 0, 320, 240, clear_color);
}

void init_lcd(const lcd_cfg_t * cfg)
{
    lcd_cfg = cfg;
    gpio_set_state(lcd_cfg->spi_dev.cs_pin, 0);

    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 0);
    delay_ms(10);
    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 1);
    delay_ms(10);

    unsigned idx = 0;
    while (idx < ARRAY_SIZE(lcd_init_cmd_list)) {
        const lcd_cmd_t * cmd = (const lcd_cmd_t *) &lcd_init_cmd_list[idx];
        lcd_send_cmd_with_data(cmd);
        idx += sizeof(lcd_cmd_t) + cmd->len;
    }

    lcd_pwr(1);
    // lcd_clear();

    lcd_set_area(30, 40, 50, 60);
        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x2C, .len = 0 });
        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        gpio_list_set_state(lcd_cfg->ctrl_lines, LCD_DC, 1);

    for(int i = 0; i < 50 * 60; i++) {
        spi_write_8(lcd_cfg->spi_dev.spi, 0x15);
        spi_write_8(lcd_cfg->spi_dev.spi, 0xA5);
    }
}
