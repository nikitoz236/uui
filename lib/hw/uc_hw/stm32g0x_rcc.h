#pragma once
#include <stdint.h>
#include "rcc.h"

#define APB_BUS_DIV_NUM                 1

struct hw_rcc_cfg {
    unsigned hse_val;
    enum {
        PLL_SRC_HSI16 = 2,
        PLL_SRC_HSE = 3,
    } pll_src;
    enum {
        SYSCLK_SRC_HSI = 0,
        SYSCLK_SRC_HSE = 1,
        SYSCLK_SRC_PLL = 2,
        SYSCLK_SRC_LSI = 3,
        SYSCLK_SRC_LSE = 4,
    } sysclk_src;
    uint8_t pll_div;                    // PLLM
    uint8_t pll_mul;                    // PLMN
    hclk_div_t hclk_div;
    apb_div_t apb_div[APB_BUS_DIV_NUM];
};
