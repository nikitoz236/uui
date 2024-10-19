#include "config.h"

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = {GPIO_PORT_A, 10},
    .tx_pin = {GPIO_PORT_A, 9},
    .tx_dma = {
        .dma_ch = 4,
    },
    .rx_dma = {
        .dma_ch = 5,
    },
    .pclk = {PCLK_BUS_APB2, RCC_APB2ENR_USART1EN},
    .irqn = USART1_IRQn
};

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
