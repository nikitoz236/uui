#include "bl_pwm.h"

static pwm_cfg_t * pwm;

void init_backlight(backlight_cfg_t * cfg)
{
    pwm = &cfg->pwm;
    init_pwm(pwm);
}

void bl_set(unsigned lvl)
{
    pwm_set_ccr(pwm, lvl);
}
