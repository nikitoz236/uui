#pragma once
#include "color_type.h"

typedef struct __attribute__((packed)) {
    lcd_color_t fg;
    lcd_color_t bg;
} color_scheme_t;

static inline color_scheme_t color_scheme_inverted(color_scheme_t * cs)
{
    return (color_scheme_t) { .fg = cs->bg, .bg = cs->fg };
}
