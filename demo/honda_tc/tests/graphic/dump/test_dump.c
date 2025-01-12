#include <stdio.h>
#include "emu_tc.h"
#include "dlc_poll.h"

#include "widget_tc_dump.h"

ui_node_desc_t ui = {
    .widget = &__widget_tc_dump
};

int main()
{
    printf("test tc dump screen\r\n");
    emu_ui_node(&ui);
    return 0;
}
