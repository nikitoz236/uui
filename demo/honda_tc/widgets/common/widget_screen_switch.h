#pragma once
#include "ui_tree.h"
#include "text_field.h"

extern const widget_desc_t widget_screen_switch;

typedef struct {
    unsigned screens_num;
    ui_node_desc_t * screens_list;

    text_field_t * title_cfg;
    const char ** titles;
} widget_screen_switch_cfg_t;
