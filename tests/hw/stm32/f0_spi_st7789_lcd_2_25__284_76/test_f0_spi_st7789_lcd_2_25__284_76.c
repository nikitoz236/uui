/*
    тест 2.25" TFT модуля на контроллере ST7789 (76 x 284 RGB) на STM32F051 (LQFP32)

    параметры модуля:
        диагональ       : 2.25"
        разрешение      : 76 x 284 RGB565
        контроллер      : Sitronix ST7789
        надпись         : Estardyn 2.25" TFT LCD ST7789
        polarizer       : transmissive (требуется подсветка для видимости)

    в RAM ST7789 240 x 320 — видимая зона 76 x 284 центрирована,
    стартовая колонка = (240 -  76) / 2 = 82
    стартовая строка  = (320 - 284) / 2 = 18

    распиновка модуля (8-pin) и подключение:

        модуль                STM32F051 (LQFP32)    комментарий
        ──────                ──────────────────    ───────────
         GND   ─── GND
         VCC   ─── 3.3 V
         SCL   ───  PB3  AF0                        SPI1 SCK
         SDA   ───  PB5  AF0                        SPI1 MOSI
         RST   ───  PB0
         DC    ───  PB7
         CS    ───  PB6
         BL    ───  PB1 (TIM14_CH1 AF0)             PWM подсветка

    после init залит чёрный, рисуем 4 угловых маркера разных цветов —
    проверяет смещение, ориентацию и порядок цветов за один взгляд:

        верх-лево  : красный
        верх-право : зелёный
        низ-лево   : синий
        низ-право  : белый
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
#include "api_lcd_color.h"

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
        .width = 76,
        .height = 284,
        .x_offset = 82,
        .y_offset = 18,
        .xy_swap = 1,
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
            .clock_div = SPI_DIV_4,
            .spi = SPI1,
            .pclk = PCLK_SPI1,
            .dma_tx_ch = 3,
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

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    init_systick();
    __enable_irq();

    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);

    lcd_bl(5);

    unsigned w = lcd_cfg.gcfg.width;
    unsigned h = lcd_cfg.gcfg.height;
    unsigned mark = 20;

    lcd_rect(0,         0,         mark, mark, COLOR(0xFF0000));
    lcd_rect(w - mark,  0,         mark, mark, COLOR(0x00FF00));
    lcd_rect(0,         h - mark,  mark, mark, COLOR(0x0000FF));
    lcd_rect(w - mark,  h - mark,  mark, mark, COLOR(0xFFFFFF));

    while (1) {};

    return 0;
}
