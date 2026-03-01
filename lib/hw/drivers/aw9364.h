#pragma once

#include "backlight.h"
#include "gpio.h"

struct backlight_cfg {
    gpio_t * pin;
};
