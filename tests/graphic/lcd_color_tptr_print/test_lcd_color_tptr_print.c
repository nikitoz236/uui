#include <stdio.h>
#include <stdint.h>

#include "emu_lcd.h"
#include "emu_graphic.h"

#include "lcd_text_color.h"

int main()
{
    printf("test lcd_color_tptr_print\r\n");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 240, .h = 135 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    form_t lcd_form;

    emu_lcd_init(&lcd_cfg, &lcd_form);
    emu_graphic_init_xy(lcd_form.s);
    emu_lcd_clear();

    lcd_rect(4, 5, 200, 98, 0x1234);


    extern const font_t font_5x7;

    lcd_font_cfg_t fcfg = {
        .font = &font_5x7,
        .gaps = 0,
        .scale = 2
    };

    color_scheme_t cs = {.bg = 0x1234, .fg = 0xA234};

    tptr_t tp;
    text_ptr_init(&tp, &fcfg, (xy_t){8, 9}, (xy_t){20, 8});

    text_ptr_set_char_pos(&tp, (xy_t){ 3, 4 });
    lcd_color_tptr_print(&tp, "lol", cs, 0);

    text_ptr_next_str(&tp);
    lcd_color_tptr_print(&tp, "kek", cs, 0);

    text_ptr_set_char_pos(&tp, (xy_t){ 6, 2 });
    lcd_color_tptr_print(&tp, "cheburek", cs, 0);

    lcd_color_tptr_print(&tp, "aga", cs, 0);

    emu_graphic_loop(0);

    return 0;
}
