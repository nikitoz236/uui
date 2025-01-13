#pragma once

#include "lcd_text.h"
#include "val_text.h"
#include "forms_align.h"

typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t limit_char;
    xy_t margin;
    xy_t padding;       // ???
    align_t a;
} text_field_t;

typedef struct {
    const text_field_t * tfcfg;
    char * text;
    xy_t pos;
    uint16_t len;
} text_field_label_t;

typedef struct {
    const text_field_t * tfcfg;
    const char * (*to_str)(unsigned x);
    const val_text_t * vt;
    xy_t pos;
    uint16_t len;
} text_field_value_t;

static inline xy_t text_field_size_px(const text_field_t * cfg)
{
    return size_add_padding(lcd_text_size_px(cfg->fcfg, cfg->limit_char), cfg->padding);
}

static inline xy_t text_field_text_pos(const form_t * f, const text_field_t * cfg)
{
    return align_form_pos(f, lcd_text_size_px(cfg->fcfg, cfg->limit_char), cfg->a, cfg->padding);
}
