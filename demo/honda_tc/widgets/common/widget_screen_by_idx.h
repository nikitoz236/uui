#pragma once
#include "ui_tree.h"

extern const widget_desc_t widget_screen_by_idx;

typedef struct {
    ui_node_desc_t * screens_list;
    unsigned screens_num;
} widget_screen_by_idx_cfg_t;
