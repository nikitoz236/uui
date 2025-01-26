#pragma once
#include <stdint.h>
#include "rcc.h"

#define APB_BUS_DIV_NUM                 1

struct rcc_cfg {
    unsigned hse_val;
    enum {
        PLL_SRC_HSI_DIV2,
        PLL_SRC_PREDIV,
    } pll_src;
    enum {
        SYSCLK_SRC_HSI = 0,
        SYSCLK_SRC_HSE = 1,
        SYSCLK_SRC_PLL = 2,
    } sysclk_src;
    uint8_t pll_prediv;
    uint8_t pll_mul;
    hclk_div_t hclk_div;
    apb_div_t apb_div[APB_BUS_DIV_NUM];
};
