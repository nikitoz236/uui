#include <stdio.h>
#include <stdint.h>

#include "emu_graphic.h"

#include "ui_lt.h"
#include "widget__test_text.h"

#include "dp.h"

ui_lt_node_desc_t ui_desc = {
    .widget = &widget__test_text
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

    emu_graphic_loop(0);

    return 0;
}
