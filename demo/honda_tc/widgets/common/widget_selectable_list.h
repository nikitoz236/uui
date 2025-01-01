#pragma once
#include "ui_tree.h"

/*
    виджет позволяет расположить несколько виджетов в виде списка
    и выбирать один из них двигаясь вверх вниз
*/

extern const widget_desc_t __widget_selectable_list;

typedef struct {
    uint16_t num;
    uint8_t different_nodes : 1;
    const ui_node_desc_t * ui_node;
} __widget_selectable_list_cfg_t;
