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
    spi_set_frame_len(lcd_cfg->spi_slave.spi, 8);
    gpio_set_state(&lcd_cfg->dc, 0);
    spi_write_8(lcd_cfg->spi_slave.spi, cmd->cmd);

    if (cmd->len == 0) {
        return;
    }

    while (spi_is_busy(lcd_cfg->spi_slave.spi)) {};
    gpio_set_state(&lcd_cfg->dc, 1);
    spi_dma_tx_buf(lcd_cfg->spi_slave.spi, cmd->data, cmd->len);
}

const uint8_t lcd_init_cmd_list[] = {
//  cmd   len       data

    // http://www.lcdwiki.com/res/MSP2807/ILI9341%20Datasheet.pdf
    // 8.4.1 Power control A (CBh) p.195

// не обязательный блок. но кажется что цвета другие
    0xCB, 5,        0x39, 0x2C, 0x00, 0x34, 0x02,
    0xCF, 3,        0x00, 0xC1, 0x30,
    0xE8, 3,        0x85, 0x00, 0x78,
    0xEA, 2,        0x00, 0x00,
    0xED, 4,        0x64, 0x03, 0x12, 0x81,
    0xF7, 1,        0x20,
// конец не обязательного блока

    // Power control
    // VRH[5:0]
    0xC0, 1,        0x23,

    // Power control
    // SAP[2:0];BT[3:0]
    0xC1, 1,        0x10,

    // VCM control
    0xC5, 2,        0x3e, 0x28,

    // VCM control2
    0xC7, 1,        0x86,

    // Memory Access Control
    //           MY MX MV ML BGR MH 0 0
    // C8	88 = 1  0  0  0  1   0  0 0
    0x36, 1,        0xC8,

    // COLMOD: Pixel Format Set
    0x3A, 1,        0x55,

    0xB1, 2,        0x00, 0x18,

    // Display Function Control
    0xB6, 3,        0x08, 0x82, 0x27,

    // 3Gamma Function Disable
    0xF2, 1,        0x00,

    // Gamma curve selected
    0x26, 1,        0x01,

    // Set Gamma
    0xE0, 15,       0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,

    // Set Gamma
    0xE1, 15,       0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
};

void lcd_pwr(unsigned val)
{
    if (val) {
        lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x11}); // Sleep Out
        delay_ms(120);
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
    while (spi_is_busy(lcd_cfg->spi_slave.spi)) {};

    u16_to_be_buf8(tmp.start, y);
    u16_to_be_buf8(tmp.end, y + h - 1);
    tmp.cmd.cmd = 0x2A;

    lcd_send_cmd_with_data(&tmp.cmd);
    while (spi_is_busy(lcd_cfg->spi_slave.spi)) {};
}

// extern const gpio_pin_t debug_gpio_list[];
// #define __DBGPIO_LCD(n, x)   gpio_set_state(&debug_gpio_list[n], x);

#define __DBGPIO_LCD(n, x)

#define __DBGPIO_LCD_RECT(x)            __DBGPIO_LCD(0, x)
#define __DBGPIO_LCD_DMA_SEND(x)        __DBGPIO_LCD(1, x)
#define __DBGPIO_LCD_IMAGE(x)           __DBGPIO_LCD(0, x)

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color)
{
    __DBGPIO_LCD_RECT(1);
    static unsigned color_static;
    lcd_set_area(x, y, w, h);
    lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x2C, .len = 0 });
    spi_set_frame_len(lcd_cfg->spi_slave.spi, 16);
    gpio_set_state(&lcd_cfg->dc, 1);
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
        spi_dma_tx_repeat(lcd_cfg->spi_slave.spi, &color_static, chunk);
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
    gpio_set_state(&lcd_cfg->spi_slave.cs_pin, 0);

    gpio_set_state(&lcd_cfg->rst, 0);
    delay_ms(10);
    gpio_set_state(&lcd_cfg->rst, 1);
    delay_ms(10);

    unsigned idx = 0;
    while (idx < ARRAY_SIZE(lcd_init_cmd_list)) {
        const lcd_cmd_t * cmd = (const lcd_cmd_t *) &lcd_init_cmd_list[idx];
        lcd_send_cmd_with_data(cmd);
        idx += sizeof(lcd_cmd_t) + cmd->len;
    }

    lcd_pwr(1);
    lcd_clear();
}
