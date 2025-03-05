#include <stdio.h>
#include "emu_tc.h"

#include "widget_selectable_list.h"
#include "widget_route_list_item.h"

#include "routes.h"

#include "emu_storage.h"
#include "storage.h"

#include "date_time.h"
#include "rtc.h"

int time_zone_get(void)
{
    return 5 * 60 * 60;
}

uint8_t selector = 0;

ui_node_desc_t ui = {
    .widget = &widget_selectable_list,
    .cfg = &(widget_selectable_list_cfg_t){
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

    init_rtc();
    rtc_set_time_s(date_time_to_s(&(date_t){ .y = 2025, .m = MONTH_FEB, .d = 19 }, &(time_t){ .h = 11, .m = 43, .s = 12 }));

    emu_storage_load();

    storage_init();
    storage_print_info();

    route_load();

    emu_ui_node(&ui);
    return 0;
}
