#pragma once
#include "ui_tree.h"
#include "color_type.h"
#include "fonts.h"

extern const widget_desc_t __widget_text;

typedef struct {
    lcd_color_t color_bg_unselected;
    lcd_color_t color_bg_selected;
    lcd_color_t color_text;
    const font_t * font;
    char * text;
    uint8_t scale;
} widget_text_cfg_t;
