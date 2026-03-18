#pragma once
#include "ui_lt.h"
#include "color_type.h"

/*
    text widget with two colors: colors[0] = normal, colors[1] = focused
*/

extern const widget_desc_t widget__test_selectable_text;

typedef struct {
    char * text;
    lcd_color_t colors[2];
} widget_cfg__test_selectable_text_t;
