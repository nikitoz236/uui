#pragma once
#include "ui_tree.h"
#include "color_scheme_type.h"
#include "text_field.h"
#include "text_label_color.h"

extern const widget_desc_t widget_text;

typedef struct {
    const tf_cfg_t * tf_cfg;
    const label_color_t * label;
    const lcd_color_t * bg;       // unselected, selected
    unsigned label_array : 1;
    unsigned tf_array : 1;
} widget_text_cfg_t;
