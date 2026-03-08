#include <stdio.h>
#include <stdint.h>

#include "emu_graphic.h"
#include "ui_lt.h"
#include "widget__screen_switch.h"
#include "widget__test_text.h"
#include "event_list.h"
#include "dp.h"

ui_lt_node_desc_t ui_desc = {
    .widget = &widget__screen_switch,
    .cfg = &(widget__screen_switch_cfg_t){
        .screens_num = 3,
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

uint8_t ui_memory[1024];

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
    }

    if (event) {
        ui_element_t * root = (ui_element_t *)ui_memory;
        ui_node_desc(root)->widget->process(root, event);
    }
}

int main()
{
    dpn("test_widget_screen_switch");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    emu_graphic_init(&lcd_cfg);

    ui_create(ui_memory, 1024, (form_t){ .s = lcd_cfg.size }, &ui_desc);

    emu_graphic_loop(view_process);

    return 0;
}
