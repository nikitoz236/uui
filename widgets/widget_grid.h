#pragma once
#include "widgets.h"

extern widget_t __widget_grid;

typedef struct {
    struct {
        uint8_t count;
        uint8_t gap;
    } ca[2];
} form_grid_t;

typedef struct {
    uint8_t count;
    ui_node_t * node;
} grid_child_elements_t;

typedef struct __attribute__((packed)) {
    form_grid_t grid_cfg;
    grid_child_elements_t * childs;
} widget_grid_cfg_t;
