#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_pclk.h"
#include "periph_gpio.h"
#include "periph_spi.h"
#include "systick.h"
#include "stm_pwm.h"
#include "lcd_spi.h"


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

const gpio_t status_led = {
    .gpio = {
        .port = GPIO_PORT_A,
        .pin = 5
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
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
            [LCD_RST] = {
                .port = GPIO_PORT_B,
                .pin = 4
            },
            [LCD_DC] = {
                .port = GPIO_PORT_A,
                .pin = 12
            }
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .gpio = {
                .port = GPIO_PORT_A,
                .pin = 15
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
    .bl = &(pwm_cfg_t){
        .freq = 40000,
        .max_val = 10,
        .pclk = PCLK_TIM14,
        .tim = TIM14,
        .ch = 1,
        .ch_n = 0,          // разберись в чем разница, подпиши. бесит.
        .gpio = &(gpio_t) {
            .cfg = {
                .af = 0,
                // .af = 1,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP,
            },
            .gpio = {
                .port = GPIO_PORT_B,
                .pin = 1
            }
        }
    },
};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);
    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    init_gpio(&status_led);


    init_systick();
    __enable_irq();

    init_lcd_hw(&lcd_cfg);

    init_lcd(&lcd_cfg);

    lcd_bl(5);

    gpio_set_state(&status_led, 1);

    while (1) {};

    return 0;
}
