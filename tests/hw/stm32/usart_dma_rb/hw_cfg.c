#include "config.h"

#define DEBUG_USART_TX_BUF_SIZE 64

struct {    // TODO тоже в библиотеку
    rb_t rb;
    uint8_t data[DEBUG_USART_TX_BUF_SIZE];
} debug_usart_dma_tx_ctx;

void debug_usart_dma_hander(void)
{
    usart_dma_tx_end_handler(&debug_usart);
}

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 10
        },
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_NONE,
        }
    },
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 9
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    },
    .tx_dma = {
        .dma_ch = 4,
        .size = DEBUG_USART_TX_BUF_SIZE,
        .rb = &debug_usart_dma_tx_ctx.rb
    },
    .tx_dma_handler = debug_usart_dma_hander,
    .pclk = PCLK_USART1,
    .irqn = USART1_IRQn
};

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
