#pragma once
#include "periph_header.h"
#include "rcc.h"

// typedef enum {
//     HCLK_DIV1 = 0,
//     HCLK_DIV2 = 8,
//     HCLK_DIV4 = 9,
//     HCLK_DIV8 = 10,
//     HCLK_DIV16 = 11,
//     HCLK_DIV64 = 12,
//     HCLK_DIV128 = 13,
//     HCLK_DIV256 = 14,
//     HCLK_DIV512 = 15,
// } hclk_div_t;

// typedef enum {
//     APB_DIV1 = 0,
//     APB_DIV2 = 4,
//     APB_DIV4 = 5,
//     APB_DIV8 = 6,
//     APB_DIV16 = 7,
// } apb_div_t;

typedef struct {
    const rcc_cfg_t * clock_cfg;
    unsigned f_sysclk;
    unsigned f_hclk;
} rcc_ctx_t;

extern rcc_ctx_t rcc_ctx;

static inline void flash_set_acr(unsigned f)
{
    //  000 Zero wait state, if 0      < SYSCLK <= 24 MHz
    //  001 One wait state,  if 24 MHz < SYSCLK <= 48 MHz
    //  010 Two wait states, if 48 MHz < SYSCLK <= 72 MHz
    FLASH->ACR &= ~FLASH_ACR_LATENCY;

    // в файле stm32f10x.h FLASH_ACR_LATENCY_0 = 0 и обозначает 000 Zero wait state
    // в остальных нормальных заголовках FLASH_ACR_LATENCY_0 = 1 и обозначет 0 бит

    FLASH->ACR |= 1 * ((f - 1) / 24000000);
}

static inline void rcc_run_hsi(void)
{
    // Enable HSI
    RCC->CR |= RCC_CR_HSION;

    // WAIT start HSI
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {};
}

static inline void rcc_run_hse(void)
{
    // Enable HSE
    RCC->CR |= RCC_CR_HSEON;

    // WAIT start HSE
    while ((RCC->CR & RCC_CR_HSERDY) == 0) {};
}

static inline void rcc_run_pll(void)
{
    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;

    // WAIT start PLL
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {};
}

static inline void rcc_set_sysclk_src(unsigned src)
{
    uint32_t tmp = RCC->CFGR;
    tmp &= ~RCC_CFGR_SW;
    tmp |= (src * RCC_CFGR_SW_0) & RCC_CFGR_SW;
    RCC->CFGR = tmp;

    // WAIT SYSCLK switching
    while ((RCC->CFGR & RCC_CFGR_SWS) != ((src * RCC_CFGR_SWS_0) & RCC_CFGR_SWS)) {};
}

static inline void rcc_set_hclk_div(hclk_div_t div)
{
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR |= (RCC_CFGR_HPRE_0 * div) & RCC_CFGR_HPRE;
}

static inline unsigned f_hclk_calc(hclk_div_t div)
{
    static uint16_t factors[] = { 2, 4, 8, 16, 64, 128, 256, 512 };
    unsigned d = 1;
    if (div != HCLK_DIV1) {
        d = factors[div - HCLK_DIV2];
    }
    return rcc_ctx.f_sysclk / d;
}

static inline unsigned f_apb_calc(apb_div_t div)
{
    unsigned d = 1;
    if (div != APB_DIV1) {
        d = (1 << (div - 3));
    }
    return rcc_ctx.f_hclk / d;
}
