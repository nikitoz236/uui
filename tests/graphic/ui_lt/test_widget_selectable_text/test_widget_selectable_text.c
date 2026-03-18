#include "emu_graphic.h"
#include "ui_lt.h"
#include "widget__test_selectable_text.h"
#include "dp.h"

static uint8_t ui_memory[1024];

static void on_key(char key)
{
    ui_element_t * root = (ui_element_t *)ui_memory;
    if (key == 'l') ui_select(root, 1);
    if (key == 'h') ui_select(root, 0);
}

int main()
{
    dpn("test_widget_selectable_text");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    emu_graphic_init(&lcd_cfg);

    ui_lt_node_desc_t ui_desc = {
        .widget = &widget__test_selectable_text,
        .cfg = &(widget_cfg__test_selectable_text_t){
            .text = "selectable item",
            .colors = { 0x444444, 0xFF8800 },
        }
    };

    ui_create(ui_memory, sizeof(ui_memory), (form_t){ .s = lcd_cfg.size }, &ui_desc);

    emu_graphic_loop(on_key);

    return 0;
}
