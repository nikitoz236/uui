#include "emu_graphic.h"
#include "emu_lcd.h"
#include <stdio.h>

// демонстрация эмуляции цветного экрана, базовая без дерева виджетов
// самое начало что должно работать

int main()
{
    printf("test emu graphic lcd color\r\n");

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

    emu_graphic_loop(0);

    return 0;
}
