#include <stdio.h>
#include "emu_graphic.h"
#include "emu_lcd.h"
#include "ui_tree.h"
#include "widget_test_rect_with_text.h"
#include "widget_screen_switch.h"
#include "event_list.h"

void view_process(char key)
{
    unsigned event = 0;
    if (key == 'j') {
        event = EVENT_BTN_DOWN;
    } else if (key == 'k') {
        event = EVENT_BTN_UP;
    } else if (key == ' ') {
        event = EVENT_BTN_OK;
    } else if (key == 'h') {
        event = EVENT_BTN_LEFT;
    } else if (key == 'l') {
        event = EVENT_BTN_RIGHT;
    }

    ui_tree_process_event(event);
}

int main()
{
    printf("test widget_screen_switch\r\n");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    form_t lcd_form = {};
    emu_lcd_init(&lcd_cfg, &lcd_form);
    emu_graphic_init(lcd_form.s.w, lcd_form.s.h);
    emu_lcd_clear();

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
    ui_tree_init(ui_ctx, 1024, &ui, &lcd_cfg.size);
    ui_tree_draw();
    emu_graphic_loop(view_process);

    return 0;
}
