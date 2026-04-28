#pragma once
#include "color_type.h"

/*
    реализация для цветного дисплея
*/

typedef struct __attribute__((packed)) {
    lcd_color_t fg;
    lcd_color_t bg;
} text_color_t;

static inline text_color_t color_scheme_inverted(text_color_t * cs)
{
    return (text_color_t) { .fg = cs->bg, .bg = cs->fg };
}
