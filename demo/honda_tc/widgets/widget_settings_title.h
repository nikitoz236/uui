#pragma once
#include "ui_tree.h"

extern const widget_desc_t widget_settings_title;

typedef struct {
    ui_node_desc_t * screen;
    const char ** title_list;
} widget_settings_title_cfg_t;
