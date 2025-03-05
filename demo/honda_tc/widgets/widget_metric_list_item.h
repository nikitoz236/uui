#pragma once
#include "ui_tree.h"
#include "metrics_view.h"
#include "color_type.h"

extern const widget_desc_t widget_metric_list_item;

typedef struct {
    enum { METRIC_LIST_VAL, METRIC_LIST_BOOL } type;
} widget_metric_list_item_cfg_t;

/*
    цвета
        названия
        значения
        единиц
        сырые значения
        активен
        не активен
*/
