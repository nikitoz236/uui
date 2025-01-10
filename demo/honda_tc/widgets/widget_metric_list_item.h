#include "ui_tree.h"
#include "metrics_view.h"
#include "color_type.h"

extern const widget_desc_t __widget_metric_list_item;

typedef struct {
    lcd_color_t color_text;
    lcd_color_t color_value;
    lcd_color_t color_raw;
    lcd_color_t color_unit;
    lcd_color_t color_bg;
} __widget_metric_list_item_cfg_t;
