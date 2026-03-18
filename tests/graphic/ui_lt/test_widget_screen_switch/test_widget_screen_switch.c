#include "ui_test_env.h"

#include "widget__screen_switch.h"
#include "widget__test_text.h"

ui_lt_node_desc_t ui_desc = {
    .widget = &widget__screen_switch,
    .cfg = &(widget__screen_switch_cfg_t){
        .count = 3,
        .screens_list = (ui_lt_node_desc_t[]){
            {
                .widget = &widget__test_text,
                .cfg = &(widget_cfg__test_text_t){
                    .text = "screen 0",
                    .frame_color = 0xFF4444,
                }
            },
            {
                .widget = &widget__test_text,
                .cfg = &(widget_cfg__test_text_t){
                    .text = "screen 1",
                    .frame_color = 0x44FF44,
                }
            },
            {
                .widget = &widget__test_text,
                .cfg = &(widget_cfg__test_text_t){
                    .text = "screen 2",
                    .frame_color = 0x4444FF,
                }
            },
        }
    }
};

int main()
{
    dpn("test_widget_screen_switch");
    ui_test_run(&ui_desc);
    return 0;
}
