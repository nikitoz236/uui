#include "ui_tree.h"

#include "widget_titled_screen.h"
#include "widget_metric_list.h"
#include "widget_metric_list_item.h"
#include "metrics_view.h"

ui_node_desc_t debug_metrics_screen = {
    .widget = &__widget_titled_screen,
    .cfg = &(__widget_titled_screen_cfg_t){
        .screen_num = 2,
        .titles = (char * []){
            "Debug real val list",
            "Debug switches list",
        },
        .color_text = 0,
        .color_bg_unselected = 0x678913,
        .color_bg_selected = 0xfeea75,
        .screen_list = (ui_node_desc_t []){
            {
                .widget = &__widget_metric_list,
                .cfg = &(__widget_metric_list_cfg_t){
                    .ui_node = &(ui_node_desc_t){
                        .widget = &__widget_metric_list_item,
                        .cfg = &(__widget_metric_list_item_cfg_t){
                            .color_text = 0xfffa75,
                            .color_value = 0x66CFAB,
                            .color_raw = 0xff4450,
                            .color_unit = 0x00ff00,
                            .color_bg = 0
                        }
                    },
                    .borders = { .w = 4, .h = 4 },
                    .gaps = { .x = 2, .y = 4 },
                    .color_bg = 0,
                    .num = METRIC_VAR_ID_NUM
                }
            },
            {
                .widget = &__widget_metric_list,
                .cfg = &(__widget_metric_list_cfg_t){
                    .ui_node = &(ui_node_desc_t){
                        .widget = &__widget_metric_list_item,
                        .cfg = &(__widget_metric_list_item_cfg_t){
                            .color_text = 0xfffa75,
                            .color_value = 0x66CFAB,
                            .color_raw = 0xaa4450,
                            .color_unit = 0x00ff00,
                            .color_bg = 0
                        }
                    },
                    .borders = { .w = 4, .h = 4 },
                    .gaps = { .x = 2, .y = 4 },
                    .color_bg = 0,
                    .num = METRIC_VAR_ID_NUM
                }
            }
        }
    }
};

static void draw(ui_element_t * el)
{
    ui_element_t * item = ui_tree_add(el, &debug_metrics_screen, 0);
    item->f = el->f;
    ui_tree_element_draw(item);
}

widget_desc_t __widget_screen_debug_metrics = {
    .draw = draw
};
