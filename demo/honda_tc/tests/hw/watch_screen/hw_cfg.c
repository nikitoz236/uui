#include "config.h"

const hw_rcc_cfg_t hw_rcc_cfg = {
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

const lcd_cfg_t lcd_cfg = {
    .dc = {GPIO_PORT_A, 11},
    .rst = {GPIO_PORT_A, 12},
    .spi_slave = {
        .cs_pin = {GPIO_PORT_A, 4},
        .cs_pin_cfg = &(gpio_cfg_t){ .mode = GPIO_MODE_OUTPUT, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH },
        .spi = &(const spi_cfg_t){
            .spi = SPI1,
            .dma_tx_ch = 3,
            .pclk = {
                .mask = RCC_APB2ENR_SPI1EN,
                .bus = PCLK_BUS_APB2
            },
            .pin_list = {
                [SPI_PIN_SCK] = {GPIO_PORT_A, 5},
                [SPI_PIN_MISO] = {GPIO_PORT_A, 6},
                [SPI_PIN_MOSI] = {GPIO_PORT_A, 7}
            },
            .pin_cfg = {
                [SPI_PIN_SCK] = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH },
                [SPI_PIN_MISO] = { .mode = GPIO_MODE_AF, .pull = GPIO_PULL_NONE },
                [SPI_PIN_MOSI] = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH }
            },
            // .clock_div = SPI_DIV_16
            .clock_div = SPI_DIV_2
        }
    },
    .bl = &(pwm_cfg_t){
        .freq = 40000,
        .max_val = 10,
        .ch = 2 - 1,
        .tim = TIM3,
        .tim_pclk = {
            .mask = RCC_APB1ENR_TIM3EN,
            .bus = PCLK_BUS_APB1
        },
        .gpio = { GPIO_PORT_B, 5 },
    }
};
