#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_pwm.h"

#include "lcd_spi.h"

#include "api_lcd_color.h"
// #include "lcd_text_color.h"
#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

spi_cfg_t lcd_spi = {
    .spi = &GPSPI2,
    .gpio_list = &(gpio_list_t){
        .cfg = { .mode = GPIO_MODE_SIG_IO },
        .count = 2,
        .pin_list = {
            [SPI_PIN_MOSI] = { .pin = 34, .signal = FSPID_OUT_IDX },
            [SPI_PIN_SCK] = { .pin = 35, .signal = FSPICLK_OUT_IDX }
        }
    }
};

const lcd_cfg_t lcd_cfg = {
    .no_reset = 1,
    .ctrl_lines = &(gpio_list_t){
        .cfg = { .mode = GPIO_MODE_OUT },
        .count = 1,
        .pin_list = {
            [LCD_DC] = { .pin = 37 }
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_OUT },
            .pin = { .pin = 38 }
        },
        .spi = &lcd_spi,
    },
    // .bl = & (pwm_cfg_t) {
    //     .gpio = & (gpio_t) {
    //         .cfg = { .mode = GPIO_MODE_SIG_OUT },
    //         .pin = { .pin = 38, .signal = LEDC_LS_SIG_OUT0_IDX }
    //     },
    //     .out_ch = 0,
    //     .tim_ch = 0,
    //     .duty_res = 6
    // },
    .gcfg = {
        .height = 222,
        .width = 480,
        .x_offset = 0,
        .y_offset = 49,
        .x_flip = 1,
        .y_flip = 1,

    }
};

gpio_t bl = {
    .cfg = { .mode = GPIO_MODE_OUT },
    .pin = { .pin = 42 }
};

void init_bl(void)
{
    init_gpio(&bl);
}

#include "delay_blocking.h"

void bl_set(unsigned lvl)
{
    if (lvl >= 16) {
        lvl = 0;
    }

    if (lvl) {
        gpio_set_state(&bl, 1);
        lvl--;
    }

    while (lvl--) {
        delay_us(2);
        gpio_set_state(&bl, 0);
        delay_us(2);
        gpio_set_state(&bl, 1);
    }
}

int main(void)
{
    init_bl();
    // for (unsigned i = 0; i <= 16; i++) {
    //     bl_set(i);
    //     dp("bl lvl "); dpd(i, 2); dn();
    //     delay_ms(2000);
    // }

    bl_set(5);

    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);

    // lcd_bl(30);

    lcd_rect(0, 0, 100, 40, 0x81F8);
    lcd_rect(100, 100, 100, 40, 0x8F10);

    lcd_rect(20, 80, 100, 10, COLOR(0xFF0000));
    lcd_rect(30, 100, 100, 10, COLOR(0x00FF00));
    lcd_rect(40, 120, 100, 10, COLOR(0x0000FF));

    // extern font_t font_5x7;

    // lcd_text_cfg_t tcfg;

    // tcfg.font = &font_5x7;

    // tcfg.gaps.x = 2;
    // tcfg.gaps.y = 2;
    // tcfg.scale = 2;

    // tcfg.text_size.w = 16;
    // tcfg.text_size.h = 1;


    // lcd_text_color_print(
    //     "Hey bitch!",
    //     &(xy_t){.x = 10, .y = 20 },
    //     &tcfg,
    //     &(color_scheme_t){ .bg = 0x8F10, .fg = COLOR(0x1122AA)},
    //     0, 0, 0
    // );


    while (1) {};
}


