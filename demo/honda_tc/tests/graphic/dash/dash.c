#include <stdio.h>
#include "emu_tc.h"
#include "widget_dash.h"
#include "dlc_emu.h"

ui_node_desc_t ui = {
    .widget = &widget_dash,
};

int main()
{
    printf("test dash screen\r\n");
    emu_engine_ctrl(1);
    emu_ui_node(&ui);
    return 0;
}
