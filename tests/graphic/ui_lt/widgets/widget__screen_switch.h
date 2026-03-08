#pragma once
#include "ui_lt.h"

extern const widget_desc_t widget__screen_switch;

typedef struct {
    unsigned screens_num;
    ui_lt_node_desc_t * screens_list;
} widget__screen_switch_cfg_t;
