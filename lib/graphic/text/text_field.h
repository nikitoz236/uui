#pragma once

#include "xy_type.h"
#include "font_config.h"

typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t pos;           // положение первого символа
    xy_t lim;           // размеры поля в символах
} tf_t;
