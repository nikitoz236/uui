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
        .bg_color = 0x568901
    };

    form_t lcd_form;

    emu_lcd_init(&lcd_cfg, &lcd_form);
    emu_graphic_init_xy(lcd_form.s);
    emu_lcd_clear();

    extern font_t font_5x7;

    lcd_color_text_raw_print(
        "Hey bitch!",
        &(lcd_font_cfg_t){
            .font = &font_5x7,
        },
        &(color_scheme_t){
            .bg = 0x332211,
            .fg = 0xAABBCC,
        },
        &(xy_t){ .x = 10, .y = 20},
        0,
        0,
        0
    );

    lcd_color_text_raw_print(
        "Hey bitch!",
        &(lcd_font_cfg_t){
            .font = &font_5x7,
        },
        &(color_scheme_t){
            .bg = 0x332211,
            .fg = 0xAABBCC,
        },
        &(xy_t){ .x = 10, .y = 30},
        &(xy_t){ .x = 8, .y = 4},
        &(xy_t){ .x = 5 },
        20
    );

    lcd_color_text_raw_print(
        0,
        &(lcd_font_cfg_t){
            .font = &font_5x7,
        },
        &(color_scheme_t){
            .bg = 0x332211,
            .fg = 0xAABBCC,
        },
        &(xy_t){ .x = 10, .y = 160},
        0,
        0,
        8
    );


    emu_graphic_loop(0);

    return 0;
}
