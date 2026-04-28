#include "print_char.h"
#include "api_lcd_color.h"

void print_char(char c, xy_t pos, const font_t * font, text_color_t color, unsigned scale)
{
    unsigned use_scale = scale;
    if (use_scale == 0) {
        use_scale = 1;
    }

    if (c == ' ') {
        unsigned w = (unsigned)font->size.w * use_scale;
        unsigned h = (unsigned)font->size.h * use_scale;
        lcd_rect(pos.x, pos.y, w, h, color.bg);
        return;
    }

/*
    планируется что интерфейс работы с экраном может быть асинхронным
    хотя бы на уровне того что вызов lcd_image инициализирует DMA и возвращает управление
    а значит данные для отправки в экран должны быть расположены в памяти статически
    для того чтобы можно было готовить следующий столбик символа пока передается предыдущий
    нам нужно два буфера чтобы их чередовать
*/
    #if !defined MAX_FONT_HEIGHT
        #define MAX_FONT_HEIGHT 40
    #endif
    static lcd_color_t char_col_buffer[2][MAX_FONT_HEIGHT];

    unsigned col_step;
    const uint8_t * font_data = font_char_ptr(c, font, &col_step);
    if (font_data != 0) {
        for (int rx = 0; rx < font->size.w; rx++) {
            lcd_color_t * col_buf = char_col_buffer[rx & 1];
            uint8_t mask = 1;
            for (int ry = 0; ry < font->size.h; ry++) {
                if (*font_data & mask) {
                    col_buf[ry] = color.fg;
                } else {
                    col_buf[ry] = color.bg;
                }
                if (mask == 0x80) {
                    mask = 1;
                    font_data++;
                } else {
                    mask <<= 1;
                }
            }
            if (mask != 1) {
                font_data++;
            }

            lcd_image(pos.x + (rx * use_scale), pos.y, 1, font->size.h, use_scale, col_buf);
        }
    } else {
        // если символа нет рисуем пиксельную шашечку
        lcd_color_t * col_buf = char_col_buffer[0];

        // сначала заполняем col_buf на 1 пиксель больше высоты символа, цветами фона и текста
        unsigned ry = 0;
        while (ry < (font->size.h + 1)) {
            col_buf[ry + 0] = color.fg;
            col_buf[ry + 1] = color.bg;
            ry += 2;
        }

        // дальше заполняем знакоместо столбиками из буфера, сдвигая на 1 пиксель между сосоедними столбиками
        for (int rx = 0; rx < font->size.w; rx++) {
            lcd_image(pos.x + rx * use_scale, pos.y, 1, font->size.h, use_scale, &col_buf[rx & 1]);
        }
    }
}
