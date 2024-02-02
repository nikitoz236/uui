#pragma once
#include "ui_tree.h"
#include "forms.h"

extern const widget_desc_t __widget_layout_tree;

typedef struct {
    ui_node_desc_t * node_list;
    xy_t * size_list;
    uint8_t * selected_layout;
    xy_t borders;
    xy_t gaps;
    uint8_t node_list_len;
    uint8_t size_list_len;
} __widget_layout_tree_cfg_t;
