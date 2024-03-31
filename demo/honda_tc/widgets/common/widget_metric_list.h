#include "ui_tree.h"
#include "color_type.h"

extern widget_desc_t __widget_metric_list;

typedef struct {
    ui_node_desc_t * ui_node;
    xy_t borders;
    xy_t gaps;
    lcd_color_t color_bg;
    uint8_t num;
} __widget_metric_list_cfg_t;
