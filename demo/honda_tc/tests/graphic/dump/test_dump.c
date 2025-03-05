#include <stdio.h>
#include "emu_tc.h"
#include "dlc_poll.h"

#include "widget_dlc_dump.h"
#include "widget_screen_switch.h"

ui_node_desc_t ui = {
    .widget = &widget_screen_switch,
    .cfg = &(widget_screen_switch_cfg_t){
        .screens_num = 1,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &widget_dlc_dump
            }
        },
    }
};

int main()
{
    printf("test tc dump screen\r\n");
    emu_ui_node(&ui);
    return 0;
}
