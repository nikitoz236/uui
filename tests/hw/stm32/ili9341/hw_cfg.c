#include "config.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 9,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV2,
        APB_DIV1
    }
};

pwm_cfg_t lcd_bl_cfg = {
    .freq = 40000,
    .max_val = 10,
    .ch = 2 - 1,
    .tim = TIM3,
    .pclk = PCLK_TIM3,
    .gpio = &(gpio_t){
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 5
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP
        }
    }
};

spi_cfg_t spi_cfg = {
    .spi = SPI1,
    .dma_tx_ch = 3,
    .pclk = PCLK_SPI1,
    .pin_list = {
        [SPI_PIN_SCK] = &(gpio_t){ .gpio = {GPIO_PORT_A, 5}, .cfg = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH }},
        [SPI_PIN_MISO] = &(gpio_t){ .gpio = {GPIO_PORT_A, 6}, .cfg = { .mode = GPIO_MODE_AF, .pull = GPIO_PULL_NONE }},
        [SPI_PIN_MOSI] = &(gpio_t){ .gpio = {GPIO_PORT_A, 7}, .cfg = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH }},
    },
    // .clock_div = SPI_DIV_64
    .clock_div = SPI_DIV_2
};

const lcd_cfg_t lcd_cfg = {
    .ctrl_lines = &(gpio_list_t){
        .cfg = {
            .mode = GPIO_MODE_OUTPUT,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP
        },
        .count = 2,
        .pin_list = (gpio_pin_t[]){
            [LCD_DC] = {GPIO_PORT_A, 11},
            [LCD_RST] = {GPIO_PORT_A, 12}
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .gpio = {GPIO_PORT_A, 4},
            .cfg = {
                .mode = GPIO_MODE_OUTPUT,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP
            },
        },
        .spi = &(spi_cfg_t){
            .spi = SPI1,
            .dma_tx_ch = 3,
        .pclk = PCLK_SPI1,
            .pin_list = {
                [SPI_PIN_SCK] = &(gpio_t){ .gpio = {GPIO_PORT_A, 5}, .cfg = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH }},
                [SPI_PIN_MISO] = &(gpio_t){ .gpio = {GPIO_PORT_A, 6}, .cfg = { .mode = GPIO_MODE_AF, .pull = GPIO_PULL_NONE }},
                [SPI_PIN_MOSI] = &(gpio_t){ .gpio = {GPIO_PORT_A, 7}, .cfg = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH }},
            },
            // .clock_div = SPI_DIV_64
            .clock_div = SPI_DIV_2
        },
    },
    .bl = &(pwm_cfg_t){
        .freq = 40000,
        .max_val = 10,
        .ch = 2,
        .tim = TIM3,
        .pclk = PCLK_TIM3,
        .gpio = &(gpio_t){
            .gpio = {
                .port = GPIO_PORT_B,
                .pin = 5
            },
            .cfg = {
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP
            }
        }
    },
};
