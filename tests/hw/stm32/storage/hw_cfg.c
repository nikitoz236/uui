#include "config.h"
#include "rb.h"

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

RB_CREATE(debug_usart_dma_tx_ctx, DEBUG_USART_TX_BUF_SIZE);

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(gpio_pin_cfg_t) {
        .gpio = {GPIO_PORT_A, 10},
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_NONE,
        }
    },
    .tx_pin = &(gpio_pin_cfg_t) {
        .gpio = {GPIO_PORT_A, 9},
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
    .pclk = & (hw_pclk_t) {PCLK_BUS_APB2, RCC_APB2ENR_USART1EN},
    .irqn = USART1_IRQn
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
};
