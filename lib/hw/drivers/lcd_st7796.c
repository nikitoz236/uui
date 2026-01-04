









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
    spi_dma_tx_buf(lcd_cfg->spi_dev.spi, cmd->data, cmd->len);
}

void set_madctl(void)
{
    struct {
        lcd_cmd_t cmd;
        struct {
            // MADCTL: Memory Access Control
            //      7   6   5   4   3   2   1   0
            //      MY  MX  MV  ML  BGR MH  0   0
            uint8_t reserved: 2;
            uint8_t MH : 1;
            uint8_t BGR : 1;
            uint8_t ML : 1;
            uint8_t MV : 1;
            uint8_t MX : 1;
            uint8_t MY : 1;
        } madctl;
    } tmp = {
        .cmd = {
            .cmd = 0x36,
            .len = 1,
        },
        .madctl = {
            .MY = lcd_cfg->gcfg.y_flip,
            .MX = lcd_cfg->gcfg.x_flip,
            .MV = lcd_cfg->gcfg.xy_swap,
            .BGR = 0,
        }
    };

    lcd_send_cmd_with_data(&tmp.cmd);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
}


const uint8_t lcd_init_cmd_list[] = {
//  cmd   len       data
    // COLMOD: Pixel Format (RGB565)
    0x3A, 1,  0x55,

    0xF0, 1,        0xC3,

    0xF0, 1,        0x96,

    0xB4, 1,        0x02,

    0xB7, 1,        0xC6,

    0xC0, 2,        0xC0, 0x00,

    0xC1, 1,        0x13,

    0xC2, 1,        0xA7,

    0xC5, 1,        0x21,

    0xE8, 8,        0x40, 0x8A, 0x1B, 0x1B, 0x23, 0x0A, 0xAC, 0x33,

    0xE0, 14,       0xD2, 0x05, 0x08, 0x06, 0x05, 0x02, 0x2A, 0x44, 0x46, 0x39, 0x15, 0x15, 0x2D, 0x32,

    0xE1, 14,       0x96, 0x08, 0x0C, 0x09, 0x09, 0x25, 0x2E, 0x43, 0x42, 0x35, 0x11, 0x11, 0x28, 0x2E,

    0xF0, 1,        0x3C,

    0xF0, 1,        0x69,

    0x21, 1

};





void lcd_pwr(unsigned val)
{
    if (val) {
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x11}); // Sleep Out
        // данное время всетаки влияет, 10 мс мало. на экране картинка рассыпается. было 120.
        delay_ms(10);
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

    x += lcd_cfg->gcfg.x_offset;

    // TODO, костыли потомучто ты не хочешь тратить время на переключение SPI ?
    u16_to_be_buf8(tmp.start, x);
    u16_to_be_buf8(tmp.end, x + w - 1);
    tmp.cmd.cmd = 0x2B;

    lcd_send_cmd_with_data(&tmp.cmd);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};

    y += lcd_cfg->gcfg.y_offset;

    u16_to_be_buf8(tmp.start, y);
    u16_to_be_buf8(tmp.end, y + h - 1);
    tmp.cmd.cmd = 0x2A;

    lcd_send_cmd_with_data(&tmp.cmd);
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
}

// extern const gpio_pin_t debug_gpio_list[];
// #define __DBGPIO_LCD(n, x)   gpio_set_state(&debug_gpio_list[n], x);

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
    const lcd_color_t clear_color = 0;
    lcd_rect(0, 0, lcd_cfg->gcfg.width, lcd_cfg->gcfg.height, clear_color);
}

void init_lcd(const lcd_cfg_t * cfg)
{
    lcd_cfg = cfg;

    if (cfg->no_reset == 0) {
        gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 0);
        delay_ms(20);
        gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 1);
        delay_ms(10);
    }

    spi_dev_select(&lcd_cfg->spi_dev);

    // if (cfg->no_reset) {
    //     const lcd_cmd_t cmd_swrst = { .cmd = 1, .len = 0 };
    //     lcd_send_cmd_with_data(&cmd_swrst);
    //     delay_ms(10);
    // }


    set_madctl();
    unsigned idx = 0;
    while (idx < ARRAY_SIZE(lcd_init_cmd_list)) {
        const lcd_cmd_t * cmd = (const lcd_cmd_t *) &lcd_init_cmd_list[idx];
        lcd_send_cmd_with_data(cmd);
        idx += sizeof(lcd_cmd_t) + cmd->len;
    }

    delay_ms(120);

    lcd_pwr(1);
    lcd_clear();
}
