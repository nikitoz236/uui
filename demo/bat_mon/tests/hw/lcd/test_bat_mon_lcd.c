/*
        FPC                              STM32F051         комментарий
        ───                              ─────────         ───────────
         1  VGL    ─── C 1uF / 10V на GND                  внешний кап. на charge pump
         2  GND    ─── GND
         3  VDD    ─── 3.3 V
         4  SDA    ───  PB5  AF0                           SPI1 MOSI
         5  SCL    ───  PB3  AF0                           SPI1 SCK
         6  RS     ───  PA15                               D/C
         7  RESET  ───  xxx
         8  CS     ───  PB0
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
    .sysclk_src = SYSCLK_SRC_HSI,
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
                .port = GPIO_PORT_A,
                .pin = 15
            }
		}
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .gpio = {
                .port = GPIO_PORT_B,
                .pin = 0
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

    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    init_systick();
    __enable_irq();

    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);

    lcd_clear();
    tf_t tf = {
        .fcfg = &fcfg,
        .pos = { .x = 4, .y = 4 },
        .lim = { .x = 24, .y = 1 },
    };
    tptr_t tptr = text_ptr_create(tf);
    text_print(&tptr, "POWER BOX MONITOR:", (text_color_t){ .inverted = 0 }, 0);
    lcd_refresh();

    lcd_bl(5);

    while (1) {};

    return 0;
}
