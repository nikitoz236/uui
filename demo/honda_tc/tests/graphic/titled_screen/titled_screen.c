#include <stdio.h>
#include "ui_tree.h"
#include "emu_lcd.h"

#include "widget_test_rect_with_text.h"
#include "widget_titled_screen.h"

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
    printf("test widget titled screen\r\n");

    __widget_emu_lcd_cfg_t emu_lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    emu_lcd_init(&emu_lcd_cfg);



    ui_node_desc_t ui = {
        .widget = &__widget_titled_screen,
        .cfg = &(__widget_titled_screen_cfg_t){
            .title = "Titled screen",
            .cs = (color_scheme_t){
                .bg = 0x00ff00,
                .fg = 0x000000
            },
            .screen = &(ui_node_desc_t){
                .widget = &__widget_test_rect_with_text,
                .cfg = &(__widget_test_rect_with_text_cfg_t){
                    .text = "Hello world",
                    .cs = (color_scheme_t){
                        .bg = 0xA2345B,
                        .fg = 0x123457
                    },
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
