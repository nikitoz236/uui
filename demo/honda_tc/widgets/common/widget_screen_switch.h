#pragma once
#include "ui_tree.h"
#include "text_field.h"

extern const widget_desc_t widget_screen_switch;

typedef struct {
    ui_node_desc_t * screens_list;
    uint8_t screens_num;
    uint8_t different_nodes : 1;

    tf_cfg_t * title_cfg;
    const char ** titles;
} widget_screen_switch_cfg_t;
