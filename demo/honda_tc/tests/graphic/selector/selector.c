#include <stdio.h>
#include "ui_tree.h"
#include "emu_lcd.h"

#include "widget_screen_switch.h"
#include "widget_selectable.h"

void view_process(char key)
{
    unsigned event = 0;
    if (key == 'j') {
        event = 1;
    } else if (key == 'k') {
        event = 2;
    } else if (key == ' ') {
        event = 10;
    } else if (key == 'h') {
        event = 3;
    } else if (key == 'l') {
        event = 4;
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
    ui_tree_init(ui_ctx, 1024, &ui, &emu_lcd_cfg.size);

    ui_tree_draw();

    // ui_tree_debug_print_tree();

    emu_lcd_loop(view_process);

    return 0;
}
