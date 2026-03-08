#pragma once
#include "ui_lt.h"
#include "color_type.h"

/*
    simple text widget for tests
*/

extern const widget_desc_t widget__test_text;

typedef struct  {
    char * text;
    lcd_color_t frame_color;
} widget_cfg__test_text_t;

