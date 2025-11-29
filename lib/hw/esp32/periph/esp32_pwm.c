#include "esp32_pwm.h"
#include "pwm.h"

#include "soc/ledc_struct.h"
#include "soc/system_reg.h"
#include "esp32_pclk.h"


void pwm_set_freq(const pwm_cfg_t * cfg, unsigned f)
{

}

void pwm_set_ccr(const pwm_cfg_t * cfg, unsigned val)
{
    LEDC.channel_group[0].channel[cfg->out_ch].duty.duty = val << 4;
    LEDC.channel_group[0].channel[cfg->out_ch].conf1.duty_start = 1;
    LEDC.channel_group[0].channel[cfg->out_ch].conf0.low_speed_update = 1;
}

void init_pwm(const pwm_cfg_t * cfg)
{
    pclk_t ledc_pclk = SYSTEM_LEDC_CLK_EN_S;
    pclk_ctrl(ledc_pclk, 1);

    init_gpio(cfg->gpio);

    LEDC.conf.apb_clk_sel = 1;

    LEDC.timer_group[0].timer[cfg->tim_ch].conf.duty_resolution = cfg->duty_res;
    LEDC.timer_group[0].timer[cfg->tim_ch].conf.clock_divider = 625 << 8;
    LEDC.timer_group[0].timer[cfg->tim_ch].conf.low_speed_update = 1;

    LEDC.channel_group[0].channel[cfg->out_ch].hpoint.hpoint = 0;
    // LEDC.channel_group[0].channel[cfg->out_ch].conf0.idle_lv = 1;
    LEDC.channel_group[0].channel[cfg->out_ch].conf0.sig_out_en = 1;

    pwm_set_ccr(cfg, 0);

    LEDC.timer_group[0].timer[cfg->tim_ch].conf.rst = 0;
}
