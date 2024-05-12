#include "stm32f10x.h"
#include "rcc_stm32f10x.h"

static void rcc_run_hsi(void)
{
    // Enable HSI
    RCC->CR |= RCC_CR_HSION;

    // WAIT start HSI
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {};
}

static void rcc_run_hse(void)
{
    // Enable HSE
    RCC->CR |= RCC_CR_HSEON;

    // WAIT start HSE
    while ((RCC->CR & RCC_CR_HSERDY) == 0) {};
}

static void rcc_set_pllmul(int pll_mull)
{
    if ((pll_mull > 16) || (pll_mull < 2)) {
        return;
    }

    // Set PLLMUL
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL_0 * (pll_mull - 2);
}

static void rcc_run_pll(void)
{
    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;

    // WAIT start PLL
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {};
}

static void rcc_set_sysclk_src(sysclk_src_t src)
{
    uint32_t tmp;

    if (src > SYSCLK_SRC_PLL) {
        return;
    }

    // Select SYSCLK froom PLL
    tmp = RCC->CFGR;
    tmp &= ~RCC_CFGR_SW;
    tmp |= src * RCC_CFGR_SW_0;
    RCC->CFGR = tmp;

    // WAIT SYSCLK switching
    while ((RCC->CFGR & RCC_CFGR_SWS) != (src * RCC_CFGR_SWS_0)) {};
}

static void rcc_set_pll_src_hsi_div2(void)
{

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

static struct hw_rcc_cfg * clock_cfg;
static unsigned f_sysclk = 8000000;     // HSI
static unsigned f_hclk = 8000000;

static unsigned f_hclk_calc(hclk_div_t div)
{
    static uint16_t factors[] = { 2, 4, 8, 16, 64, 128, 256, 512 };
    unsigned d = 1;
    if (div != HCLK_DIV1) {
        d = factors[div - 8];
    }
    return f_sysclk / d;
}

static unsigned f_apb_calc(apb_div_t div)
{
    unsigned d = 1;
    if (div != APB_DIV1) {
        d = (1 << (div - 3));
    }
    return f_hclk / d;
}

void hw_rcc_apply_cfg(struct hw_rcc_cfg * cfg)
{
    clock_cfg = cfg;
    if (clock_cfg->sysclk_src == SYSCLK_SRC_PLL) {
        if (clock_cfg->pll_src == PLL_SRC_PREDIV) {
            rcc_run_hse();
            rcc_set_pll_src_prediv(clock_cfg->pll_prediv);
        } else if (clock_cfg->pll_src == PLL_SRC_HSI_DIV2) {
            rcc_set_pll_src_hsi_div2();
        }
        rcc_set_pllmul(clock_cfg->pll_mul);
        rcc_run_pll();
        f_sysclk = clock_cfg->hse_val * clock_cfg->pll_mul / clock_cfg->pll_prediv;
    } else if (clock_cfg->sysclk_src == SYSCLK_SRC_HSE) {
        rcc_run_hse();
        f_sysclk = clock_cfg->hse_val;
    } else if (clock_cfg->sysclk_src == SYSCLK_SRC_HSI) {
        rcc_run_hsi();
        f_sysclk = 8000000;
        // может чтото еще ??
    }
    rcc_set_sysclk_src(clock_cfg->sysclk_src);

    RCC->CFGR &= ~(0xF * RCC_CFGR_HPRE_0);
    RCC->CFGR |= clock_cfg->hclk_div * RCC_CFGR_HPRE_0;

    f_hclk = f_hclk_calc(cfg->hclk_div);

    for (unsigned i = 0; i < PCLK_APB_BUS_NUM; i++) {
        static const uint32_t apb_divs_fields[] = {
            RCC_CFGR_PPRE1_0,
            RCC_CFGR_PPRE2_0
        };

        RCC->CFGR &= ~(0x7 * apb_divs_fields[i]);
        RCC->CFGR |= clock_cfg->apb_div[i] * apb_divs_fields[i];
    }
}

void hw_rcc_pclk_ctrl(struct hw_pclk * pclk, unsigned state)
{
    static const __IO uint32_t * rcc_enr[] = {
        &RCC->APB1ENR,
        &RCC->APB2ENR,
        &RCC->AHBENR
    };

    __IO uint32_t * rcc_enr_reg = rcc_enr[pclk->bus];

    if (state) {
        *rcc_enr_reg |= pclk->mask;
    } else {
        *rcc_enr_reg &= ~pclk->mask;
    }
}

unsigned hw_rcc_f_hclk(void)
{
    return f_hclk;
}

unsigned hw_rcc_f_pclk(enum pclk_bus bus)
{
    return f_apb_calc(clock_cfg->apb_div[bus]);
}

unsigned hw_rcc_f_timer(enum pclk_bus bus)
{
    unsigned f_timer = hw_rcc_f_pclk(bus);
    if (clock_cfg->apb_div[bus] != APB_DIV1) {
        f_timer *= 2;
    }
    return f_timer;
}
