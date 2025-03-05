#include "ui_tree.h"

#include "widget_dlc_dump.h"

// uint8_t screen_selector = 0;

ui_node_desc_t ui = {
    .widget = &__widget_tc_dump
};

uint8_t ui_ctx[1024] = {0};

void init_ui(void)
{
    ui_tree_init(ui_ctx, 1024, &ui, &(xy_t){ .w = 320, .h = 240});
    ui_tree_draw();
}
