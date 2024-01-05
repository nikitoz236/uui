#pragma once
#include "widgets.h"
#include "fonts.h"
#include "color_type.h"

extern widget_t __widget_text_color;

typedef struct text_color_scheme_t {
    lcd_color_t fg;
    lcd_color_t bg;
} color_scheme_t;

typedef struct __attribute__((packed)) {
    color_scheme_t * cs;
    char * text;
} widget_text_color_ctx_t;

typedef struct __attribute__((packed)) {
    xy_t text_size;
    font_t * font;
    xy_t gaps;
    uint8_t scale;
} widget_text_color_cfg_t;
