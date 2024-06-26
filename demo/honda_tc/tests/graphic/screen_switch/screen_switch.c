#include <stdio.h>
#include "ui_tree.h"
#include "emu_lcd.h"

#include "widget_test_rect_with_text.h"
#include "widget_screen_switch.h"

void view_process(char key)
{
    unsigned event = 0;
    if (key == 'k') {
        event = 1;
    } else if (key == 'j') {
        event = 2;
    }

    ui_tree_process_event(event);
}

int main()
{
    printf("test honda widget metrics list\r\n");

    __widget_emu_lcd_cfg_t emu_lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    emu_lcd_init(&emu_lcd_cfg);

    unsigned screen_selector = 0;

    ui_node_desc_t ui = {
        .widget = &__widget_screen_switch,
        .cfg = &(__widget_screen_switch_cfg_t){
            .selector_ptr = &screen_selector,
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

    ui_node_desc_t ui_test = {
        .widget = &__widget_test_rect_with_text,
        .cfg = &(__widget_test_rect_with_text_cfg_t){
            .text = "text2",
            .cs = {
                .bg = 0x708090,
                .fg = 0x23fa75
            }
        }
    };


    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, &ui, &emu_lcd_cfg.size);

    ui_tree_draw();

    ui_tree_debug_print_tree();


    emu_lcd_loop(view_process);

    return 0;
}
