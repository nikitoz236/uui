#include "lcd_text_color.h"
#include "api_lcd_color.h"

// обработает нам одно знакоместо
static inline void print_char(char c, unsigned x, unsigned y, const font_t * font, lcd_color_t fg, lcd_color_t bg, unsigned scale)
{
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

    uint8_t col_step;
    const uint8_t * font_data = font_char_ptr(c, font, &col_step);
    if (font_data != 0) {
        for (int rx = 0; rx < font->size.w; rx++) {
            lcd_color_t * col_buf = char_col_buffer[rx & 1];
            uint8_t mask = 1;
            for (int ry = 0; ry < font->size.h; ry++) {
                if (*font_data & mask) {
                    col_buf[ry] = fg;
                } else {
                    col_buf[ry] = bg;
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

            lcd_image(x + (rx * scale), y, 1, font->size.h, scale, col_buf);
        }
    } else {
        lcd_color_t * col_buf = char_col_buffer[0];
        for (int ry = 0; ry < (font->size.h + 1);) {
            col_buf[ry] = fg;
            ry++;
            col_buf[ry] = bg;
            ry++;
        }

        for (int rx = 0; rx < font->size.w; rx++) {
            lcd_image(x + rx, y, 1, font->size.h, scale, &col_buf[rx & 1]);
        }
    }
}

static inline void fill_rem_form_str(unsigned x, unsigned y, unsigned rem_chars, lcd_text_cfg_t * cfg, unsigned color)
{
    unsigned rem_w = (rem_chars * cfg->font->size.w * cfg->scale) + ((rem_chars - 1) * cfg->gaps.w);
    lcd_rect(x, y, rem_w, cfg->font->size.h * cfg->scale, color);
}

void lcd_text_color_print(char * c, xy_t * pos, lcd_text_cfg_t * cfg, color_scheme_t * cs, unsigned tx, unsigned ty, unsigned len)
{
    unsigned scale = cfg->scale;
    unsigned char_count = 0;

    // unsigned y = pos->y;
    // for (int cy = 0; cy < cfg->text_size.h; cy++) {
        // unsigned x = pos->x;
        // for(int cx = 0; cx < cfg->text_size.w ; cx++) {

    unsigned y = pos->y + ((cfg->font->size.y * cfg->scale) + cfg->gaps.y) * ty;
    for (int cy = ty; cy < cfg->text_size.h; cy++) {
        unsigned x = pos->x + ((cfg->font->size.x * cfg->scale) + cfg->gaps.x) * tx;
        for(int cx = tx; cx < cfg->text_size.w ; cx++) {
            // если возврат каретки     то игнорируем его               увеличиваем указатель       не выходим из цикла
            // если 0                   то рисуем до конца формы фон    не увеличиваем указатель    выходим из цикла
            // если перевод строки      то рисуем до конца формы фон    увеличиваем указатель       выходим из цикла
            // если пробел              то рисуем до конца символа фон  увеличиваем указатель       не выходим из цикла

            // внутри print_char()
            // если символ в таблице    то рисуем его                   увеличиваем указатель       не выходим из цикла
            // если не в таблице        то рисуем шашечку               увеличиваем указатель       не выходим из цикла
            // utf 8 потом надо бы ....

            printf("text draw font. y %d, cy %d, of %d\n", y, cy, cfg->text_size.h);

            if (*c == '\r') {
                c++;
            }

            lcd_color_t bg = cs->bg;
            if (*c == 0) {
                bg = 0xFF0000;
                if (len == 0) {
                    fill_rem_form_str(x, y, cfg->text_size.w - cx, cfg, bg);
                }
                cx = cfg->text_size.w;
            } else if (*c == '\n') {
                bg = 0xFFFF00;
                fill_rem_form_str(x, y, cfg->text_size.w - cx, cfg, bg);
                cx = cfg->text_size.w;
                c++;
            } else {
                if (*c == ' ') {
                    bg = 0x00FF00;
                    lcd_rect(x, y, (cfg->font->size.w * scale) + cfg->gaps.w, (cfg->font->size.h * scale), bg);
                    x += cfg->font->size.w * cfg->scale + cfg->gaps.w;
                } else {
                    // lcd_rect(x, y, cfg->font->size.w * cfg->scale, cfg->font->size.h * scale, ctx->cs->fg);
                    print_char(*c, x, y, cfg->font, cs->fg, cs->bg, scale);
                    x += cfg->font->size.w * cfg->scale;
                    if (cx != (cfg->text_size.w - 1)) {
                        bg = 0xFF00FF;
                        lcd_rect(x, y, cfg->gaps.w, cfg->font->size.h * scale, bg);
                    }
                    x += cfg->gaps.w;
                }
                c++;
            }
            char_count++;
            if (char_count == len) {
                return;
            }
            tx = 0;
        }
        y += cfg->font->size.h * cfg->scale;

        printf("text draw gap y %d, cy %d, of %d\n", y, cy, cfg->text_size.h);
        if (cy != (cfg->text_size.h - 1)) {
            lcd_rect(pos->x, y, pos->w, cfg->gaps.h, 0xFFFF00);
            printf("   drawed\n");
        }
        y += cfg->gaps.h;
    }
};
