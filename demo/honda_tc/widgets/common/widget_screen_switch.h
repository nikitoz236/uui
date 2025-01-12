#pragma once
#include "ui_tree.h"
#include "text_field.h"

extern widget_desc_t __widget_screen_switch;

typedef struct {
    unsigned screens_num;
    ui_node_desc_t * screens_list;
    uint8_t * selector_ptr;
    text_field_t * title_cfg;
    const char ** titles;
} __widget_screen_switch_cfg_t;
