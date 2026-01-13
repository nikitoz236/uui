#pragma once
#include "xy_type.h"
#include "fonts.h"

typedef struct {
    const font_t * font;
    xy_t gaps;              // вот это как раз какбы маргин
    uint8_t scale;          // досихпор не понятно как влияет на gaps - не влияет, за счет этого большая точность, можно некратные зазоры делать
} lcd_font_cfg_t;

// эти 2 функции нужны потомучто мне лень иногда заполнять структуры и 0 должен превратиться в 1, зазора и скейла 0 быть не может
unsigned fcfg_scale(const lcd_font_cfg_t * cfg);
unsigned fcfg_gap(const lcd_font_cfg_t * cfg, dimension_t d);

xy_t fcfg_text_size_px(const lcd_font_cfg_t * cfg, xy_t text_size_chars);
xy_t fcfg_text_char_places(const lcd_font_cfg_t * cfg, xy_t size_px);
