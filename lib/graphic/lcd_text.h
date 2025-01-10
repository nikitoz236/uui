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

typedef struct {
    const font_t * font;
    xy_t gaps;
    uint8_t scale;
} lcd_font_cfg_t;

void lcd_text_calc_size(xy_t * size, const lcd_text_cfg_t * cfg);

xy_t lcd_text_size_px(const lcd_font_cfg_t * cfg, xy_t text_size_chars);
xy_t lcd_text_char_places(const lcd_font_cfg_t * cfg, xy_t size_px);

void lcd_text_extend_text_size(const xy_t * available_size, lcd_text_cfg_t * cfg);
void lcd_text_extend_scale(const xy_t * available_size, lcd_text_cfg_t * cfg);
void lcd_text_extend_gaps(const xy_t * available_size, lcd_text_cfg_t * cfg);
