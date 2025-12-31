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

    lcd_rect(4, 5, 200, 98, 0xA234);





    emu_graphic_loop(0);

    return 0;
}
