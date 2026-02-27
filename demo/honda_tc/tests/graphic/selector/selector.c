#include <stdio.h>
#include "ui_tree.h"
#include "emu_graphic.h"
#include "emu_lcd.h"

#include "event_list.h"
#include "widget_screen_switch.h"
#include "widget_selectable.h"


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

    ui_tree_process(event);
}

int main()
{
    printf("test honda widget metrics list\r\n");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    form_t lcd_form = {};
    emu_lcd_init(&lcd_cfg, &lcd_form);
    emu_graphic_init_xy(lcd_form.s);
    emu_lcd_clear();

    uint8_t screen_selector = 0;

    ui_node_desc_t ui = {
        .widget = &__widget_screen_switch,
        .cfg = &(__widget_screen_switch_cfg_t){
            .selector_ptr = &screen_selector,
            .screens_num = 4,
            .screens_list = (ui_node_desc_t[]){
                {
                    .widget = &__widget_selectable,
                },
                {
                    .widget = &__widget_selectable,
                },
                {
                    .widget = &__widget_selectable,
                },
                {
                    .widget = &__widget_selectable,
                }
            }
        }
    };

    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, &ui, &lcd_cfg.size);
    ui_tree_draw();

    // ui_tree_debug_print_tree();

    emu_graphic_loop(view_process);

    return 0;
}
