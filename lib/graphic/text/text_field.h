#pragma once

#include "xy_type.h"
#include "font_config.h"
#include "forms_split.h"

typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t pos;           // положение первого символа
    xy_t lim;           // размеры поля в символах
} tf_t;

static inline tf_t tf_create(const lcd_font_cfg_t * fcfg, form_t f, xy_t padding)
{
    form_t padded = form_cut_padding(f, padding);
    return (tf_t) {
        .fcfg = fcfg,
        .pos = padded.p,
        .lim = fcfg_text_char_places(fcfg, padded.s)
    };
}
