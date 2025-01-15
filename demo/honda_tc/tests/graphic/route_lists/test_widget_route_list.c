#include <stdio.h>
#include "emu_tc.h"

#include "widget_selectable_list.h"
#include "widget_route_list_item.h"

#include "routes.h"

int time_zone_get(void)
{
    return 5 * 60 * 60;
}

uint8_t selector = 0;

ui_node_desc_t ui = {
    .widget = &__widget_selectable_list,
    .cfg = &(__widget_selectable_list_cfg_t){
        .different_nodes = 0,
        .margin = { .x = 2, .y = 2 },
        .num = ROUTE_TYPE_NUM,
        .ui_node = &(ui_node_desc_t){
            .widget = &widget_route_list_item
        },
    }
};

int main()
{
    printf("test route lists\r\n");
    emu_ui_node(&ui);
    return 0;
}
