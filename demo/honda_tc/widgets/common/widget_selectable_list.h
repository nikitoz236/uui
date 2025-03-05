#pragma once
#include "ui_tree.h"

/*
    виджет позволяет расположить несколько виджетов в виде списка
    и выбирать один из них двигаясь вверх вниз
*/

extern const widget_desc_t widget_selectable_list;

typedef struct {
    uint16_t num;
    xy_t margin;
    const ui_node_desc_t * ui_node;
    uint8_t different_nodes : 1;
    uint8_t selectable : 1;
    uint8_t cycled : 1;
} widget_selectable_list_cfg_t;
