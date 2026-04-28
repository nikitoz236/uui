#include "emu_graphic.h"
#include "emu_fb_lcd.h"
#include "lcd_fb.h"
#include "print_char.h"
#include "api_lcd_color.h"
#include "dp.h"

extern const font_t font_3x5;
extern const font_t font_5x5;
extern const font_t font_5x7;

LCD_FB_CREATE(192, 96);

typedef struct {
    char c;
    coord_t x;
    coord_t y;
    const font_t * font;
    unsigned scale;
} char_args_t;

static const char_args_t test_chars[] = {
    // font_3x5
    { '0', 0, 0, &font_3x5, 1 },
    { '1', 5, 1, &font_3x5, 1 },
    { '2', 10, 3, &font_3x5, 1 },
    { 'b', 15, 5, &font_3x5, 1 },
    { 'A', 21, 2, &font_3x5, 2 },
    { '5', 31, 0, &font_3x5, 3 },
    { ':', 44, 2, &font_3x5, 2 },
    // font_5x5
    { 'A', 0, 16, &font_5x5, 1 },
    { 'B', 7, 17, &font_5x5, 1 },
    { 'C', 14, 19, &font_5x5, 1 },
    { 'D', 21, 21, &font_5x5, 1 },
    { 'E', 30, 18, &font_5x5, 2 },
    { 'F', 46, 16, &font_5x5, 3 },
    // font_5x7
    { 'A', 0, 32, &font_5x7, 1 },
    { 'b', 7, 33, &font_5x7, 1 },
    { 'c', 14, 35, &font_5x7, 1 },
    { 'D', 21, 37, &font_5x7, 1 },
    { 'E', 30, 34, &font_5x7, 2 },
    { 'f', 46, 32, &font_5x7, 3 },
    { '\x01', 70, 32, &font_5x7, 2 },
    // нижний ряд — большие scale через границу 8-px page
    { 'Z', 0, 56, &font_5x7, 3 },
    { 'Y', 20, 59, &font_5x7, 3 },
    { 'X', 40, 62, &font_5x7, 3 },
};

int main(void)
{
    dpn("test mono print_char");

    emu_fb_lcd_cfg_t cfg = {
        .scale = 4,
        .px_gap = 1,
        .border = 8,
        .bg_color = 0x5f7537,
        .on_color = 0,
        .off_color = 0x526137,
        .msb_top = 0,
        .flip_180 = 0,
        .px_byte_debug = 1,
        .px_byte_debug_color = 0x112233
    };
    emu_lcd_mono_init(&cfg);

    lcd_clear();
    lcd_rect(96, 0, 96, 96, 1);

    unsigned n = sizeof(test_chars) / sizeof(test_chars[0]);
    for (unsigned inverted = 0; inverted < 2; inverted++) {
        text_color_t color = { .inverted = inverted };
        coord_t x_off = 0;
        if (inverted) {
            x_off = 100;
        }
        for (unsigned i = 0; i < n; i++) {
            const char_args_t * t = &test_chars[i];
            xy_t pos = { .x = t->x + x_off, .y = t->y };
            print_char(t->c, pos, t->font, color, t->scale);
        }
    }

    lcd_refresh();

    emu_graphic_loop(0);
    return 0;
}
