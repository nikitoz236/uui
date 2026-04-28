/*
    тест экрана SSD1306 (128x64 OLED) на STM32F051 (LQFP32)
    цель — дёрнуть инициализацию, увидеть мусор из RAM на экране

    подключение модуля (маркировка пинов снизу платы):

        модуль SSD1306        STM32F051
        ─────────────         ─────────
            GND          ───  GND
            VCC          ───  3.3 V
            D0  (SCK)    ───  PB3   AF0
            D1  (MOSI)   ───  PB5   AF0
            RES (RST)    ───  PB1
            DC           ───  PB7
            CS           ───  PB6
*/

#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "systick.h"
#include "periph_spi.h"

#include "lcd_spi.h"
#include "lcd_fb.h"
#include "api_lcd_color.h"
#include "text_print.h"
#include "delay_blocking.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 6,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV1,
    }
};

const lcd_cfg_t lcd_cfg = {
    .ctrl_lines = &(gpio_list_t){
        .cfg = {
            .mode = GPIO_MODE_OUTPUT,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        },
        .count = 2,
        .pin_list = (gpio_pin_t[]){
            [LCD_DC] = {
                .port = GPIO_PORT_B,
                .pin = 7
            },
            [LCD_RST] = {
                .port = GPIO_PORT_B,
                .pin = 1
            },
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .gpio = {
                .port = GPIO_PORT_B,
                .pin = 6
            },
            .cfg = {
                .mode = GPIO_MODE_OUTPUT,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP,
            }
        },
        .spi = &(spi_cfg_t){
            .clock_div = SPI_DIV_64,
            .spi = SPI1,
            .pclk = PCLK_SPI1,
            .pin_list = {
                [SPI_PIN_SCK] = &(gpio_t){
                    .gpio = {
                        .port = GPIO_PORT_B,
                        .pin = 3
                    },
                    .cfg = {
                        .mode = GPIO_MODE_AF,
                        .speed = GPIO_SPEED_HIGH,
                        .type = GPIO_TYPE_PP,
                        .af = 0
                    }
                },
                [SPI_PIN_MOSI] = &(gpio_t){
                    .gpio = {
                        .port = GPIO_PORT_B,
                        .pin = 5
                    },
                    .cfg = {
                        .mode = GPIO_MODE_AF,
                        .speed = GPIO_SPEED_HIGH,
                        .type = GPIO_TYPE_PP,
                        .af = 0
                    }
                },
            }
        }
    },
};

LCD_FB_CREATE(128, 64);

extern const font_t font_5x5;

static const lcd_font_cfg_t fcfg = {
    .font = &font_5x5,
    .gaps = { .x = 1, .y = 1 },
    .scale = 1,
};

int main()
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    init_systick();
    __enable_irq();

    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);

    lcd_clear();
    tf_t tf = {
        .fcfg = &fcfg,
        .pos = { .x = 10, .y = 28 },
        .lim = { .x = 13, .y = 1 },
    };
    tptr_t tptr = text_ptr_create(tf);
    text_print(&tptr, "BRIGHTNESS: ", (text_color_t){ .inverted = 0 }, 0);
    lcd_refresh();

    unsigned lvl = 0;
    while (1) {
        char digit = '0' + lvl;
        text_ptr_set_char_pos(&tptr, (xy_t){ .x = 12, .y = 0 });
        text_print(&tptr, &digit, (text_color_t){ .inverted = 0 }, 1);

        lcd_refresh();
        lcd_bl(lvl);

        delay_ms(1000);

        lvl++;
        if (lvl > 8) {
            lvl = 0;
        }
    }

    return 0;
}
