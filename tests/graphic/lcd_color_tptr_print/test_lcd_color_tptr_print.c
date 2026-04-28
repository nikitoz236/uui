#include <stdio.h>
#include <stdint.h>

#include "emu_graphic.h"

#include "text_print.h"

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

    emu_graphic_init(&lcd_cfg);

    lcd_rect(4, 5, 200, 98, 0x1234);


    extern const font_t font_5x7;

    lcd_font_cfg_t fcfg = {
        .font = &font_5x7,
        .scale = 2
    };

    text_color_t color = { .bg = 0x1234, .fg = 0xA234 };

    tptr_t tp = text_ptr_create((tf_t){ .fcfg = &fcfg, .pos = (xy_t){8, 9}, .lim = (xy_t){20, 8} } );

    text_ptr_set_char_pos(&tp, (xy_t){ 3, 4 });
    text_print(&tp, "lol", color, 0);

    text_ptr_next_str(&tp);
    text_print(&tp, "kek", color, 0);

    text_ptr_set_char_pos(&tp, (xy_t){ 6, 2 });
    text_print(&tp, "cheburek", color, 0);

    text_print(&tp, "aga", color, 0);

    emu_graphic_loop(0);

    return 0;
}
