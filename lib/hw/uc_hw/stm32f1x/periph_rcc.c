#include "../stm_rcc_common.h"
#include "periph_rcc.h"
#include "pclk.h"

rcc_ctx_t rcc_ctx;

static void rcc_set_pllmul(unsigned pll_mull)
{
    if ((pll_mull > 16) || (pll_mull < 2)) {
        return;
    }

    // Set PLLMUL
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL_0 * (pll_mull - 2);
}

static void rcc_set_pll_src_prediv(unsigned prediv)
{
    // Set PLL src
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE;

    if ((prediv > 2) || (prediv == 0)) {
        return;
    }

    // Set PREDIV
    RCC->CFGR &= ~RCC_CFGR_PLLXTPRE_HSE_Div2;
    RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE_Div2 * (prediv - 1);
}

static void rcc_set_apb_div(unsigned bus, apb_div_t div)
{
    static const uint32_t apb_divs_fields[] = {
        RCC_CFGR_PPRE1_0,
        RCC_CFGR_PPRE2_0
    };

    RCC->CFGR &= ~(0x7 * apb_divs_fields[bus]);
    RCC->CFGR |= (div & 0x7) * apb_divs_fields[bus];
}

void rcc_apply_cfg(const rcc_cfg_t * cfg)
{
    rcc_ctx.clock_cfg = cfg;
    if (cfg->sysclk_src == SYSCLK_SRC_PLL) {
        if (cfg->pll_src == PLL_SRC_PREDIV) {
            rcc_run_hse();
            rcc_set_pll_src_prediv(cfg->pll_prediv);
        } else if (cfg->pll_src == PLL_SRC_HSI_DIV2) {
            // rcc_set_pll_src_hsi_div2();
        }
        rcc_set_pllmul(cfg->pll_mul);
        rcc_run_pll();
        rcc_ctx.f_sysclk = (cfg->hse_val * cfg->pll_mul) / cfg->pll_prediv;
    } else if (cfg->sysclk_src == SYSCLK_SRC_HSE) {
        rcc_run_hse();
        rcc_ctx.f_sysclk = cfg->hse_val;
    } else if (cfg->sysclk_src == SYSCLK_SRC_HSI) {
        rcc_run_hsi();
        rcc_ctx.f_sysclk = HSI_VALUE;
        // может чтото еще ??
    }

    rcc_ctx.f_hclk = f_hclk_calc(cfg->hclk_div);
    flash_set_acr(rcc_ctx.f_hclk);

    rcc_set_sysclk_src(cfg->sysclk_src);
    rcc_set_hclk_div(cfg->hclk_div);

    for (unsigned i = 0; i < APB_BUS_DIV_NUM; i++) {
        rcc_set_apb_div(i, cfg->apb_div[i]);
    }
}

void pclk_ctrl(const pclk_t * pclk, unsigned state)
{
    static __IO uint32_t * const rcc_enr[] = {
        [PCLK_BUS_APB1] = &RCC->APB1ENR,
        [PCLK_BUS_APB2] = &RCC->APB2ENR,
        [PCLK_BUS_AHB] = &RCC->AHBENR,
    };

    __IO uint32_t * const rcc_enr_reg = rcc_enr[pclk->bus];

    if (state) {
        *rcc_enr_reg |= pclk->mask;
    } else {
        *rcc_enr_reg &= ~pclk->mask;
    }
}

unsigned pclk_f(const pclk_t * pclk)
{
    unsigned apb_bus_idx = pclk->bus - PCLK_BUS_APB1;
    if (apb_bus_idx >= APB_BUS_DIV_NUM) {
        return f_apb_calc(APB_DIV1);
    }
    return f_apb_calc(rcc_ctx.clock_cfg->apb_div[apb_bus_idx]);
}

unsigned pclk_f_timer(const pclk_t * pclk)
{
    unsigned f_timer = pclk_f(pclk);
    if (rcc_ctx.clock_cfg->apb_div[pclk->bus - PCLK_BUS_APB1] != APB_DIV1) {
        f_timer *= 2;
    }
    return f_timer;
}

unsigned pclk_f_hclk(void)
{
    return rcc_ctx.f_hclk;
}
