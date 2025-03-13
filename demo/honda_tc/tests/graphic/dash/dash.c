#include <stdio.h>
#include "emu_tc.h"
#include "widget_dash.h"

ui_node_desc_t ui = {
    .widget = &widget_dash,
};

int main()
{
    printf("test dash screen\r\n");
    emu_ui_node(&ui);
    return 0;
}
