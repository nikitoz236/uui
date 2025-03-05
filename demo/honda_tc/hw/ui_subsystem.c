#include "ui_tree.h"


#include "widget_screen_switch.h"
#include "widget_menu_screen.h"

ui_node_desc_t ui = {
    .widget = &widget_screen_switch,
    .cfg = &(widget_screen_switch_cfg_t){
        .screens_num = 4,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &widget_menu_screen,
            }
        }
    }
};

uint8_t ui_ctx[1024] = {0};

void init_ui(void)
{
    ui_tree_init(ui_ctx, 1024, &ui, &(xy_t){ .w = 320, .h = 240});
    ui_tree_draw();
}
