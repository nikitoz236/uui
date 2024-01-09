#pragma once
#include "forms.h"
#include "fonts.h"

/*
    здесь только про геометрию текста
*/

typedef struct __attribute__((packed)) {
    const font_t * font;
    xy_t text_size;
    xy_t gaps;
    uint8_t scale;
} lcd_text_cfg_t;

void lcd_text_calc_size(xy_t * size, lcd_text_cfg_t * cfg);
void lcd_text_extend_text_size(xy_t * available_size, lcd_text_cfg_t * cfg);
void lcd_text_extend_scale(xy_t * available_size, lcd_text_cfg_t * cfg);
