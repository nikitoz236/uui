#include "stm32f10x_pwm.h"

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
    unsigned t_psc = hw_rcc_f_timer(&cfg->tim_pclk) / (cfg->max_val * 2 * f);
    cfg->tim->PSC = t_psc - 1;
}

void pwm_set_ccr(const pwm_cfg_t * cfg, unsigned val)
{
    __IO uint16_t * ccr = &cfg->tim->CCR1;
    ccr[cfg->ch * 2] = val;
}

void init_pwm(const pwm_cfg_t * cfg)
{
    hw_rcc_pclk_ctrl(&cfg->tim_pclk, 1);
    const gpio_cfg_t gpio_cfg = {
        .mode = GPIO_MODE_AF,
        .type = GPIO_TYPE_PP,
        .speed = GPIO_SPEED_MED
    };

    gpio_set_cfg(&cfg->gpio, &gpio_cfg);

    cfg->tim->BDTR |= TIM_BDTR_MOE;
    cfg->tim->ARR = cfg->max_val -1;
    timer_configure_output(cfg->tim, cfg->ch, 6);
    pwm_set_freq(cfg, cfg->freq);
    pwm_set_ccr(cfg, 0);
    cfg->tim->CR1 |= TIM_CR1_CEN;
}
