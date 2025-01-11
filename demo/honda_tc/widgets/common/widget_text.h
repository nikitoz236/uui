#pragma once
#include "ui_tree.h"
#include "color_scheme_type.h"
#include "lcd_text.h"

extern const widget_desc_t __widget_text;

typedef struct {
    lcd_font_cfg_t * fcgf;
    char * text;
    xy_t padding;
    color_scheme_t * cs_selected;
    color_scheme_t * cs_unselected;
} widget_text_cfg_t;
