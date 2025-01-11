#include <stdio.h>
#include "emu_tc.h"

#include "widget_selectable_list.h"
#include "widget_metric_list_item.h"

ui_node_desc_t ui = {
    .widget = &__widget_selectable_list,
    .cfg = &(__widget_selectable_list_cfg_t){
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
        .margin = { .x = 0, .y = 0 },
        .num = METRIC_VAR_ID_NUM,
        .different_nodes = 0
    }
};

int main()
{
    printf("test_widget_metric_list\r\n");
    emu_ui_node(&ui);
    return 0;
}
