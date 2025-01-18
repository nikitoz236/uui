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

unsigned pclk_f_hclk(void)
{
    return rcc_ctx.f_hclk;
}
