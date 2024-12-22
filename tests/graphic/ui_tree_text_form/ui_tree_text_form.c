#include <stdio.h>
#include <stdint.h>

#include "emu_lcd.h"
#include "emu_graphic.h"

#include "ui_tree.h"
#include "widget_test_rect_with_text.h"

int main()
{
    printf("test widget_test_rect_with_text\r\n");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    form_t lcd_form;

    emu_lcd_init(&lcd_cfg, &lcd_form);
    emu_graphic_init(lcd_form.s.w, lcd_form.s.h);
    emu_lcd_clear();

    ui_node_desc_t ui = {
        .widget = &__widget_test_rect_with_text,
        .cfg = &(__widget_test_rect_with_text_cfg_t){
            .text = "first",
            .cs = {
                .bg = 0xaabbcc,
                .fg = 0x332211
            }
        }
    };

    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, &ui, &lcd_cfg.size);
    ui_tree_draw();

    emu_graphic_loop(0);

    return 0;
}
