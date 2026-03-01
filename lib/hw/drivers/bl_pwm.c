#include "bl_pwm.h"

void init_backlight(backlight_cfg_t * cfg)
{
    init_pwm(cfg->pwm);
}

void bl_set(backlight_cfg_t * cfg, unsigned lvl)
{
    pwm_set_ccr(cfg->pwm, lvl);
}
