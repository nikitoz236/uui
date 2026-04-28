#include "emu_graphic.h"
#include "emu_fb_lcd.h"
#include "lcd_fb.h"
#include "api_lcd_color.h"
#include "text_print.h"
#include "dp.h"

extern const font_t font_5x7;

LCD_FB_CREATE(192, 96);

int main(void)
{
    dpn("test mono text_print");

    emu_fb_lcd_cfg_t cfg = {
        .scale = 4,
        .px_gap = 1,
        .border = 8,
        .bg_color = 0x5f7537,
        .on_color = 0,
        .off_color = 0x526137,
        .msb_top = 0,
        .flip_180 = 0,
    };
    emu_lcd_mono_init(&cfg);

    lcd_clear();
    // тёмный квадрат справа для инверсии
    lcd_rect(96, 0, 96, 96, 1);

    lcd_font_cfg_t fcfg = {
        .font = &font_5x7,
        .gaps = { .x = 1, .y = 1 },
        .scale = 1,
    };

    text_color_t normal = { .inverted = 0 };
    text_color_t inv    = { .inverted = 1 };

    // нормальный tptr слева
    tptr_t tp = text_ptr_create((tf_t){
        .fcfg = &fcfg,
        .pos = { .x = 2, .y = 2 },
        .lim = { .x = 14, .y = 10 },
    });

    text_print(&tp, "Hello", normal, 0);
    text_ptr_next_str(&tp);
    text_print(&tp, "world!", normal, 0);

    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 3 });
    text_print(&tp, "Line\nbreak", normal, 0);

    // len больше длины строки — хвост дополняется пробелами
    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 6 });
    text_print(&tp, "Pad", normal, 7);

    // инвертированный tptr справа (на тёмном квадрате)
    tptr_t tp_inv = text_ptr_create((tf_t){
        .fcfg = &fcfg,
        .pos = { .x = 100, .y = 2 },
        .lim = { .x = 12, .y = 10 },
    });

    text_print(&tp_inv, "Hello", inv, 0);
    text_ptr_next_str(&tp_inv);
    text_print(&tp_inv, "world!", inv, 0);

    text_ptr_set_char_pos(&tp_inv, (xy_t){ .x = 0, .y = 3 });
    text_print(&tp_inv, "Line\nbreak", inv, 0);

    text_ptr_set_char_pos(&tp_inv, (xy_t){ .x = 0, .y = 6 });
    text_print(&tp_inv, "Pad", inv, 7);

    lcd_refresh();

    emu_graphic_loop(0);
    return 0;
}
