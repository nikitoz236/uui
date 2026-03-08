#include <stdio.h>
#include <stdint.h>

#include "emu_graphic.h"

#include "ui_lt.h"
#include "widget__test_text.h"

#include "dp.h"

ui_lt_node_desc_t ui_desc = {
    .widget = &widget__test_text, .cfg = &(widget_cfg__test_text_t){.frame_color = COLOR(0xAABBCC)}
};

int main()
{
    dpn("test_widget_test_text");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    emu_graphic_init(&lcd_cfg);

    uint8_t ui_memory[1024];

    ui_create(ui_memory, 1024, (form_t){ .s = lcd_cfg.size }, &ui_desc);

    dp("widget ctx size: ");
    dpd(**((ui_element_t *)ui_memory)->lt.ctx_size, 6);
    dp(" ui_element size: ");
    dpd(sizeof(ui_element_t), 5);
    dp(" lt_item_t size: ");
    dpd(sizeof(lt_item_t), 5);
    dp(" sizeof(0): ");
    dpd(sizeof(0), 5);

    dn();

    emu_graphic_loop(0);

    return 0;
}
