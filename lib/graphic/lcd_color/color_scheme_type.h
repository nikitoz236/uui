#pragma once
#include "color_type.h"

typedef struct __attribute__((packed)) {
    lcd_color_t fg;
    lcd_color_t bg;
} color_scheme_t;
