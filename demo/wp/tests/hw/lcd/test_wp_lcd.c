#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_pclk.h"
#include "periph_gpio.h"
#include "periph_spi.h"
#include "systick.h"
#include "stm_pwm.h"
#include "lcd_spi.h"

#include "api_lcd_color.h"

#include "stm_usart.h"

#define DP_NOTABLE
#include "dp.h"

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

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 2000000,
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 6
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 0
        }
    },
    .pclk = PCLK_USART1,
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);
    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    init_gpio(&status_led);
    usart_set_cfg(&debug_usart);

    init_systick();
    __enable_irq();

    dp("Test WP LCD");

    init_lcd_hw(&lcd_cfg);

    init_lcd(&lcd_cfg);

    lcd_bl(5);

    gpio_set_state(&status_led, 1);

    lcd_rect(4, 5, 80, 120, 0xA234);

    lcd_rect(100, 5, 30, 50, COLOR(0xFF0000));
    lcd_rect(170, 5, 30, 50, COLOR(0x00FF00));



    while (1) {};

    return 0;
}
