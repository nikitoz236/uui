#include <stdio.h>
#include "emu_tc.h"

#include "widget_test_rect_with_text.h"
#include "widget_screen_switch.h"
#include "event_list.h"

int main()
{
    printf("test widget_screen_switch\r\n");

    ui_node_desc_t ui = {
        .widget = &widget_screen_switch,
        .cfg = &(widget_screen_switch_cfg_t){
            .screens_num = 3,
            .screens_list = (ui_node_desc_t[]){
                {
                    .widget = &__widget_test_rect_with_text,
                    .cfg = &(__widget_test_rect_with_text_cfg_t){
                        .text = "text1",
                        .cs = {
                            .bg = 0x708090,
                            .fg = 0xfffa75
                        }
                    }
                },
                {
                    .widget = &__widget_test_rect_with_text,
                    .cfg = &(__widget_test_rect_with_text_cfg_t){
                        .text = "text2",
                        .cs = {
                            .bg = 0xA04020,
                            .fg = 0x23fa75
                        }
                    }
                },
                {
                    .widget = &__widget_test_rect_with_text,
                    .cfg = &(__widget_test_rect_with_text_cfg_t){
                        .text = "another text",
                        .cs = {
                            .bg = 0xA04020,
                            .fg = 0x23fa75
                        }
                    }
                }
            }
        }
    };

    emu_ui_node(&ui);
    return 0;
}
