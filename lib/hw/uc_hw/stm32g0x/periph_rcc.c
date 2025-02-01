#include "../stm_rcc_common.h"
#include "periph_rcc.h"
#include "pclk.h"

rcc_ctx_t rcc_ctx;

#define HSI_VALUE                       16000000

static void rcc_set_pll_mul(unsigned mul)
{
    if ((mul > 86) || (mul < 8)) {
        return;
    }

    // Set PLLN
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_0 * mul;
}

static void rcc_set_pll_div(unsigned div)
{

    if ((div > 8) || (div < 1)) {
        return;
    }

    // Set PLLM
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_0 * (div - 1);
}

static void rcc_set_apb_div(unsigned bus, apb_div_t div)
{
    static const uint32_t apb_divs_fields[] = {
        RCC_CFGR_PPRE_0
    };

    RCC->CFGR &= ~(0x7 * apb_divs_fields[bus]);
    RCC->CFGR |= (div & 0x7) * apb_divs_fields[bus];
}

void rcc_apply_cfg(const rcc_cfg_t * cfg)
{
    rcc_ctx.clock_cfg = cfg;
    if (cfg->sysclk_src == SYSCLK_SRC_PLL) {
        // Set PLL src
        RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
        if (cfg->pll_src == PLL_SRC_HSE) {
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
            rcc_run_hse();
        } else if (cfg->pll_src == PLL_SRC_HSI16) {
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
        }
        rcc_set_pll_div(cfg->pll_div);
        rcc_set_pll_mul(cfg->pll_mul);

        RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_0 * (cfg->pll_sys_div - 1);

        RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

        rcc_run_pll();

        rcc_ctx.f_sysclk = (cfg->hse_val * cfg->pll_mul) / (cfg->pll_div * cfg->pll_sys_div);
    } else if (cfg->sysclk_src == SYSCLK_SRC_HSE) {
        rcc_run_hse();
        rcc_ctx.f_sysclk = cfg->hse_val;
    } else if (cfg->sysclk_src == SYSCLK_SRC_HSI) {
        rcc_run_hsi();
        rcc_ctx.f_sysclk = HSI_VALUE;
    }

    rcc_ctx.f_hclk = f_hclk_calc(cfg->hclk_div);

    // ну вот нужно установить до того как мы переключимся на высокую частоту
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
        [PCLK_BUS_APB1] = &RCC->APBENR1,
        [PCLK_BUS_APB2] = &RCC->APBENR2,
        [PCLK_BUS_AHB] = &RCC->AHBENR,
        [PCLK_BUS_IOP] = &RCC->IOPENR
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
