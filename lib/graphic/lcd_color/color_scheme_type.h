#pragma once
#include "color_type.h"

typedef struct __attribute__((packed)) {
    lcd_color_t fg;
    lcd_color_t bg;
} color_scheme_t;

static inline void invert_color_scheme(color_scheme_t * cs, color_scheme_t * inv)
{
    inv->fg = cs->bg;
    inv->bg = cs->fg;
}
