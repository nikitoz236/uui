#include <stdio.h>
#include <stdint.h>

#include "emu_lcd.h"
#include "emu_graphic.h"

#include "lcd_text_color.h"

int main()
{
    printf("test lcd_text_color_print\r\n");

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

    lcd_rect(4, 5, 310, 228, 0x1234);
    lcd_rect(34, 25, 45, 28, 0x4567);

    extern font_t font_5x7;
    lcd_text_color_print(
        "Hey bitch!",
        &(xy_t){.x = 50, .y = 60 },
        &(lcd_text_cfg_t){
            .font = &font_5x7,
            .gaps = { .x = 2, .y = 2 },
            .scale = 2,
            .text_size = { .w = 16, .h = 1 }
        },
        &(color_scheme_t){ .bg = 0xABCD, .fg = 0x166D},
        0, 0, 0
    );

    emu_graphic_loop(0);

    return 0;
}
