#pragma once

#include "backlight.h"
#include "pwm.h"

struct backlight_cfg {
    pwm_cfg_t pwm;
};
