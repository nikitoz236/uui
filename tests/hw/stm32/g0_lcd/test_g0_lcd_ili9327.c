#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "systick.h"
#include "stm_pwm.h"

#include "lcd_spi.h"
#include "periph_spi.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_HSE,
    .pll_div = 1,
    .pll_mul = 16,
    .pll_sys_div = 2,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        [PCLK_BUS_APB1] = APB_DIV1,
    }
};



/*
    ili9327
    на гребенке

    2 - BL              PA6     TIM3 CH1 AF 1
    3, 4 - GND
    5 - RESET           PB12
    6, 7 - 3v3
    17 - CS             PA4
    18 - DC             PA8
    19 - SCL            PA5     SPI1 AF0
    21 - MOSI           PA7     SPI1 AF0

*/

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
                .pin = 12
            },
            [LCD_DC] = {
                .port = GPIO_PORT_A,
                .pin = 8
            }
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .gpio = {
                .port = GPIO_PORT_A,
                .pin = 4
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
                        .port = GPIO_PORT_A,
                        .pin = 5
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
                        .port = GPIO_PORT_A,
                        .pin = 7
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
        .pclk = PCLK_TIM3,
        .tim = TIM3,
        .ch = 1,
        .gpio = &(gpio_t) {
            .cfg = {
                .af = 1,
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP,
            },
            .gpio = {
                .port = GPIO_PORT_A,
                .pin = 6
            }
        }
    },
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

    lcd_bl(9);

    while (1) {};

    return 0;
}


