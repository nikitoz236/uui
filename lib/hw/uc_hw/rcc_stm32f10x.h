#pragma once
#include "hw_rcc.h"
#include <stdint.h>

typedef enum {
    SYSCLK_SRC_HSI = 0,
    SYSCLK_SRC_HSE = 1,
    SYSCLK_SRC_PLL = 2,
} sysclk_src_t;

typedef enum {
    HCLK_DIV1 = 0,
    HCLK_DIV2 = 8,
    HCLK_DIV4 = 9,
    HCLK_DIV8 = 10,
    HCLK_DIV16 = 11,
    HCLK_DIV64 = 12,
    HCLK_DIV128 = 13,
    HCLK_DIV256 = 14,
    HCLK_DIV512 = 15,
} hclk_div_t;

typedef enum {
    APB_DIV1 = 0,
    APB_DIV2 = 4,
    APB_DIV4 = 5,
    APB_DIV8 = 6,
    APB_DIV16 = 7,
} apb_div_t;

enum pclk_bus {
    PCLK_BUS_APB1,
    PCLK_BUS_APB2,

    PCLK_BUS_NUM
};

struct hw_rcc_cfg {
    unsigned hse_val;
    enum {
        PLL_SRC_HSI_DIV2,
        PLL_SRC_PREDIV,
    } pll_src;
    sysclk_src_t sysclk_src;
    uint8_t pll_prediv;
    uint8_t pll_mul;
    hclk_div_t hclk_div;
    apb_div_t apb_div[PCLK_BUS_NUM];
};

struct hw_pclk {
    enum pclk_bus bus;
    unsigned mask;
};
