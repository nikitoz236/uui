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

void lcd_pwr(unsigned val)
{
    // if (val) {
    //     lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x11}); // Sleep Out
    //     // данное время всетаки влияет, 10 мс мало. на экране картинка рассыпается. было 120.
    //     delay_ms(40);
    //     lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x29}); // Display on
    // } else {
    //     lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x10}); // Enter Sleep Mode
    //     delay_ms(5);
    //     lcd_send_cmd_with_data(&(lcd_cmd_t){ .cmd = 0x28}); // Display off
    // }
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
    const lcd_color_t clear_color = 0xACEF;
    lcd_rect(0, 0, 320, 240, clear_color);
}


#define ST7789_SWRESET 						0x01
#define ST7789_SLPIN   						0x10
#define ST7789_SLPOUT  						0x11
#define ST7789_NORON   						0x13
#define ST7789_INVOFF  						0x20
#define ST7789_INVON   						0x21
#define ST7789_DISPOFF 						0x28
#define ST7789_DISPON  						0x29
#define ST7789_CASET   						0x2A
#define ST7789_RASET   						0x2B
#define ST7789_RAMWR   						0x2C
#define ST7789_COLMOD  						0x3A
#define ST7789_MADCTL  						0x36

#define ST7789_MADCTL_MY  				0x80
#define ST7789_MADCTL_MX  				0x40
#define ST7789_MADCTL_MV  				0x20
#define ST7789_MADCTL_ML  				0x10
#define ST7789_MADCTL_RGB 				0x00
#define ST7789_MADCTL_BGR 				0x08
#define ST7789_MADCTL_MH  				0x04

#define ST7789_ColorMode_65K    	0x50
#define ST7789_ColorMode_262K   	0x60
#define ST7789_ColorMode_12bit  	0x03
#define ST7789_ColorMode_16bit  	0x05
#define ST7789_ColorMode_18bit  	0x06
#define ST7789_ColorMode_16M    	0x07

enum type { CMD = 0, DATA = 1 };

static inline void lcd_send_8(const lcd_cfg_t * cfg, uint8_t data, enum type type)
{
    while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
    gpio_list_set_state(cfg->ctrl_lines, LCD_DC, type);
    spi_write_8(cfg->spi_dev.spi, data);
}

void init_lcd(const lcd_cfg_t * cfg)
{
    lcd_cfg = cfg;


    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 0);
    delay_ms(20);
    gpio_list_set_state(cfg->ctrl_lines, LCD_RST, 1);
    delay_ms(10);


    spi_set_frame_len(lcd_cfg->spi_dev.spi, 8);

        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_SWRESET, CMD);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);

    delay_ms(150);

        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_SLPOUT, CMD);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);

    delay_ms(250);

        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_COLMOD, CMD);
    lcd_send_8(cfg, ST7789_ColorMode_65K | ST7789_ColorMode_16bit, DATA);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);

    delay_ms(10);

        spi_dev_select(&lcd_cfg->spi_dev);

    // ST7789_MADCTL , 1,                 	// 4: Memory access ctrl (directions), 1 arg:
    //     ST7789_ROTATION,                  //    Row addr/col addr, bottom to top refresh
    lcd_send_8(cfg, ST7789_MADCTL, CMD);
    lcd_send_8(cfg, 0, DATA);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);


        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_CASET, CMD);
    lcd_send_8(cfg, 0, DATA);
    lcd_send_8(cfg, 0, DATA);
    lcd_send_8(cfg, 0, DATA);
    lcd_send_8(cfg, 239, DATA);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);


        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_RASET, CMD);
    lcd_send_8(cfg, 0, DATA);
    lcd_send_8(cfg, 0, DATA);
    lcd_send_8(cfg, 0, DATA);
    lcd_send_8(cfg, 239, DATA);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);


        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_INVON, CMD);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);

    delay_ms(10);

        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_NORON, CMD);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);

    delay_ms(10);

        spi_dev_select(&lcd_cfg->spi_dev);

    lcd_send_8(cfg, ST7789_DISPON, CMD);

        while (spi_is_busy(lcd_cfg->spi_dev.spi)) {};
        spi_dev_unselect(&lcd_cfg->spi_dev);

    delay_ms(10);

    // unsigned idx = 0;
    // while (idx < ARRAY_SIZE(lcd_init_cmd_list)) {
    //     const lcd_cmd_t * cmd = (const lcd_cmd_t *) &lcd_init_cmd_list[idx];
    //     lcd_send_cmd_with_data(cmd);
    //     idx += sizeof(lcd_cmd_t) + cmd->len;
    // }

    // lcd_pwr(1);
    // lcd_clear();
}
