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

#define DEBUG_USART_TX_BUF_SIZE 64

struct {
    struct dma_tx_ctx ctx;
    uint8_t data[DEBUG_USART_TX_BUF_SIZE];
} debug_usart_dma_tx_ctx;


const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = {GPIO_PORT_A, 10},
    .tx_pin = {GPIO_PORT_A, 9},
    .tx_dma = {
        .dma_ch = 3, // Channel 4
        .size = DEBUG_USART_TX_BUF_SIZE,
        .ctx = &debug_usart_dma_tx_ctx.ctx

    },
    .pclk = {PCLK_BUS_APB2, RCC_APB2ENR_USART1EN},
    .irqn = USART1_IRQn
};

const lcd_cfg_t lcd_cfg = {
    .dc = {GPIO_PORT_A, 11},
    .rst = {GPIO_PORT_A, 12},
    .spi_slave = {
        .cs_pin = {GPIO_PORT_A, 4},
        .cs_pin_cfg = &(gpio_cfg_t){ .mode = GPIO_MODE_OUTPUT, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH },
        .spi = &(const spi_cfg_t){
            .spi = SPI1,
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
