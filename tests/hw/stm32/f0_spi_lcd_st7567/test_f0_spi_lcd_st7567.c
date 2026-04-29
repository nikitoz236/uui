/*
    тест COG LCD модуля на контроллере ST7567 (128x64) на STM32F051 (LQFP32)

    параметры модуля:
        разрешение      : 128 x 64
        контроллер      : Sitronix ST7567
        polarizer       : transmissive (требуется подсветка для видимости)
        duty / bias     : 1/64 duty, 1/9 bias
        operating temp  : -10 .. +60 C
        storage temp    : -20 .. +70 C
        дата код шлейфа : 1929 (29 нед 2019)

    распиновка модуля (10-pin FPC) и подключение:

        FPC                              STM32F051         комментарий
        ───                              ─────────         ───────────
         1  VGL    ─── C 1uF / 10V на GND                  внешний кап. на charge pump
         2  GND    ─── GND
         3  VDD    ─── 3.3 V
         4  SDA    ───  PB5  AF0                           SPI1 MOSI
         5  SCL    ───  PB3  AF0                           SPI1 SCK
         6  RS     ───  PB7                                D/C
         7  RESET  ───  PB0
         8  CS     ───  PB6
         9  LEDA   ─── R (~100 Ом) ─── PB1 (TIM14_CH1 AF0)  PWM подсветка
        10  LEDK   ─── GND
*/

#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "systick.h"
#include "stm_pwm.h"
#include "bl_pwm.h"
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
    .gcfg = {
        .width = 128,
        .height = 64,
        .x_flip = 0,
        .y_flip = 0,
    },
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
                .pin = 0
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
    .bl = &(backlight_cfg_t){
        .pwm = &(pwm_cfg_t){
            .freq = 40000,
            .max_val = 10,
            .pclk = PCLK_TIM14,
            .tim = TIM14,
            .ch = 1,
            .gpio = &(gpio_t){
                .gpio = { .port = GPIO_PORT_B, .pin = 1 },
                .cfg = { .mode = GPIO_MODE_AF, .speed = GPIO_SPEED_HIGH, .type = GPIO_TYPE_PP, .af = 0 },
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
    lcd_rect(0,   0,  5, 5, 1);     // верхний левый
    lcd_rect(123, 0,  5, 5, 1);     // верхний правый
    lcd_rect(0,   59, 5, 5, 1);     // нижний левый
    lcd_rect(123, 59, 5, 5, 1);     // нижний правый
    tf_t tf = {
        .fcfg = &fcfg,
        .pos = { .x = 4, .y = 28 },
        .lim = { .x = 24, .y = 1 },
    };
    tptr_t tptr = text_ptr_create(tf);
    text_print(&tptr, "ST7567 128 X 64", (text_color_t){ .inverted = 0 }, 0);
    lcd_refresh();

    lcd_bl(5);

    while (1) {};

    return 0;
}
