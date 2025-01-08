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
        .dma_ch = 4,
        .size = DEBUG_USART_TX_BUF_SIZE,
        .ctx = &debug_usart_dma_tx_ctx.ctx

    },
    .pclk = {PCLK_BUS_APB2, RCC_APB2ENR_USART1EN},
    .irqn = USART1_IRQn
};
