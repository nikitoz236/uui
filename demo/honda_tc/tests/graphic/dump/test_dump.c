#include <stdio.h>
#include "emu_tc.h"
#include "dlc_poll.h"

#include "widget_dlc_dump.h"
#include "widget_screen_switch.h"

uint8_t selector = 0;

ui_node_desc_t ui = {
    .widget = &__widget_screen_switch,
    .cfg = &(__widget_screen_switch_cfg_t){
        .screens_num = 1,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &__widget_tc_dump
            }
        },
        .selector_ptr = &selector,
    }
};

int main()
{
    printf("test tc dump screen\r\n");
    emu_ui_node(&ui);
    return 0;
}
