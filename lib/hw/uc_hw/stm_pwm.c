#include "stm_pwm.h"

static void timer_configure_output(TIM_TypeDef * timer, unsigned ch, unsigned mode)
{
    uint8_t * ccmr_ch = (uint8_t*)&timer->CCMR1;
    if (ch & 2) {
        ccmr_ch += 4;
    }

    if (ch & 1) {
        ccmr_ch += 1;
    }

    *ccmr_ch |= TIM_CCMR1_CC1S_0 * 0;
    *ccmr_ch |= TIM_CCMR1_OC1M_0 * mode;
    timer->CCER |= 1 << (ch * 4);
}

void pwm_set_freq(const pwm_cfg_t * cfg, unsigned f)
{
    unsigned t_psc = pclk_f_timer(&cfg->pclk) / (cfg->max_val * f);
    cfg->tim->PSC = t_psc - 1;
}

void pwm_set_ccr(const pwm_cfg_t * cfg, unsigned val)
{
    __IO uint32_t * ccr = (__IO uint32_t *)&cfg->tim->CCR1;
    ccr[cfg->ch - 1] = val;
}

void init_pwm(const pwm_cfg_t * cfg)
{
    pclk_ctrl(&cfg->pclk, 1);
    init_gpio(cfg->gpio);

    cfg->tim->BDTR |= TIM_BDTR_MOE;
    cfg->tim->ARR = cfg->max_val -1;
    pwm_set_ccr(cfg, 0);
    timer_configure_output(cfg->tim, cfg->ch - 1, 6);
    pwm_set_freq(cfg, cfg->freq);
    cfg->tim->CR1 |= TIM_CR1_CEN;
}
