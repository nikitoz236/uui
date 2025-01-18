#include "stm_rcc_common.h"
#include "pclk.h"

rcc_ctx_t rcc_ctx;

unsigned f_hclk_calc(hclk_div_t div)
{
    static uint16_t factors[] = { 2, 4, 8, 16, 64, 128, 256, 512 };
    unsigned d = 1;
    if (div != HCLK_DIV1) {
        d = factors[div - HCLK_DIV2];
    }
    return rcc_ctx.f_sysclk / d;
}

unsigned f_apb_calc(apb_div_t div)
{
    unsigned d = 1;
    if (div != APB_DIV1) {
        d = (1 << (div - 3));
    }
    return rcc_ctx.f_hclk / d;
}

void hw_rcc_pclk_ctrl(const pclk_t * pclk, unsigned state)
{
    static __IO uint32_t * const rcc_enr[] = {
        [PCLK_BUS_APB1] = &RCC->APB1ENR,
        [PCLK_BUS_APB2] = &RCC->APB2ENR,
        [PCLK_BUS_AHB] = &RCC->AHBENR
    };

    __IO uint32_t * const rcc_enr_reg = rcc_enr[pclk->bus];

    if (state) {
        *rcc_enr_reg |= pclk->mask;
    } else {
        *rcc_enr_reg &= ~pclk->mask;
    }
}

unsigned pclk_f_hclk(void)
{
    return rcc_ctx.f_hclk;
}
