#pragma once
#include "color_scheme_type.h"
#include "ui_tree.h"

extern const widget_desc_t __widget_test_rect_with_text;

typedef struct {
    color_scheme_t cs;
    char * text;
} __widget_test_rect_with_text_cfg_t;
