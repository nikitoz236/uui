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

    unsigned col_step;
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
        // если символа нет рисуем пиксельную шашечку
        lcd_color_t * col_buf = char_col_buffer[0];

        // сначала заполняем col_buf на 1 пиксель больше высоты символа, цветами фона и текста
        unsigned ry = 0;
        while (ry < (font->size.h + 1)) {
            col_buf[ry + 0] = fg;
            col_buf[ry + 1] = bg;
            ry += 2;
        }

        // дальше заполняем знакоместо столбиками из буфера, сдвигая на 1 пиксель между сосоедними столбиками
        for (int rx = 0; rx < font->size.w; rx++) {
            lcd_image(x + rx, y, 1, font->size.h, scale, &col_buf[rx & 1]);
        }
    }
}

// заполняет rem_chars символов прямоугольником цвета color
static inline void fill_rem_form_str(unsigned x, unsigned y, unsigned rem_chars, const lcd_text_cfg_t * cfg, unsigned color)
{
    unsigned rem_w = (rem_chars * cfg->font->size.w * cfg->scale) + ((rem_chars - 1) * cfg->gaps.w);
    lcd_rect(x, y, rem_w, cfg->font->size.h * cfg->scale, color);
}

void lcd_text_color_print(const char * c, xy_t * pos, const lcd_text_cfg_t * cfg, const color_scheme_t * cs, unsigned tx, unsigned ty, unsigned len)
{
    unsigned scale = cfg->scale;
    unsigned char_count = 0;

    // unsigned y = pos->y;
    // for (int cy = 0; cy < cfg->text_size.h; cy++) {
        // unsigned x = pos->x;
        // for(int cx = 0; cx < cfg->text_size.w ; cx++) {

    lcd_color_t bg = cs->bg;
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

            // printf("text draw font. y %d, cy %d, of %d\n", y, cy, cfg->text_size.h);

            if (c != 0) {
                if (*c == '\r') {
                    c++;
                }
            }

            if ((c == 0) || (*c == 0)) {
                // bg = 0xFF0000;
                // заполняем строку до конца пустотой ( на случай если там был какойто другой текст шире )
                if (len == 0) {
                    fill_rem_form_str(x, y, cfg->text_size.w - cx, cfg, bg);
                } else {
                    fill_rem_form_str(x, y, len - char_count, cfg, bg);
                }
                cx = cfg->text_size.w;
            } else if (*c == '\n') {
                // bg = 0xFFFF00;
                fill_rem_form_str(x, y, cfg->text_size.w - cx, cfg, bg);
                cx = cfg->text_size.w;
                c++;
            } else {
                if (*c == ' ') {
                    // bg = 0x00FF00;
                    lcd_rect(x, y, (cfg->font->size.w * scale) + cfg->gaps.w, (cfg->font->size.h * scale), bg);
                    x += cfg->font->size.w * cfg->scale + cfg->gaps.w;
                } else {
                    // lcd_rect(x, y, cfg->font->size.w * cfg->scale, cfg->font->size.h * scale, ctx->cs->fg);
                    print_char(*c, x, y, cfg->font, cs->fg, cs->bg, scale);
                    x += cfg->font->size.w * cfg->scale;
                    if (cx != (cfg->text_size.w - 1)) {
                        // bg = 0xFF00FF;
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
        }
        y += cfg->font->size.h * cfg->scale;

        // printf("text draw gap y %d, cy %d, of %d\n", y, cy, cfg->text_size.h);
        if (cy != (cfg->text_size.h - 1)) {
            unsigned tw = (((cfg->font->size.x * cfg->scale) + cfg->gaps.x) * cfg->text_size.w) - cfg->gaps.x;
            // bg = 0xFF8844;
            lcd_rect(pos->x, y, tw, cfg->gaps.h, bg);
            // printf("   drawed\n");
        }
        tx = 0;
        y += cfg->gaps.h;
    }
};

void lcd_color_text_raw_print(const char * str, const lcd_font_cfg_t * cfg, const color_scheme_t * cs, const xy_t * pos_px, const xy_t * limit_chars, const xy_t * pos_chars, unsigned len)
{

    unsigned scale = cfg->scale;
    if (scale == 0) {
        scale = 1;
    }

    xy_t char_idx = {};
    if (pos_chars) {
        char_idx = *pos_chars;
    }

    // printf("lcd_color_text_raw_print str %s, len %d, x %d y %d, lim x %d, lim y %d \n", str, len, char_idx.x, char_idx.y, limit_chars->x, limit_chars->y);

    if (limit_chars) {
        if (char_idx.x >= limit_chars->x) {
            return;
        }
    }

    xy_t char_shift;
    xy_t char_pos_px;
    xy_t gap;


    for (unsigned i = 0; i < DIMENSION_COUNT; i++) {
        gap.ca[i] = cfg->gaps.ca[i];
        if (gap.ca[i] == 0) {
            gap.ca[i] = 1;
        }
        char_shift.ca[i] = (cfg->font->size.ca[i] * scale) + gap.ca[i];

        char_pos_px.ca[i] = pos_px->ca[i];

        if (pos_chars) {
            char_pos_px.ca[i] += pos_chars->ca[i] * char_shift.ca[i];
        }
    }

    // printf("lcd_color_text_raw_print str %s, len %d\n", str, len);

    while (1) {
        char c = 0;

        if (str) {
            c = *str;
            if (c) {
                str++;
            }
        }

        // printf("     char %c hex %02x\n", c, c);

        if (c == 0) {
            if (len == 0) {
                return;
            }
            // если строка закончилась или отсутствует, но указан len
            // заполняем оставшиеся от len знакоместа пробелами

            // printf("lcd_color_text_raw_print fill_rem_form_str\n");
            c = ' ';
        }

        // printf("lcd_color_text_raw_print char %c\n", c);

        if (c == ' ') {
            lcd_rect(char_pos_px.x, char_pos_px.y, (cfg->font->size.w * scale), (cfg->font->size.h * scale), cs->bg);
            // ?? надо ли заполнять зазор между буквами?
        } else {

            print_char(c, char_pos_px.x, char_pos_px.y, cfg->font, cs->fg, cs->bg, scale);
        }

        if (len) {
            len--;
            if (len == 0) {
                return;
            }
        }

        char_idx.x++;
        if ((limit_chars) && (char_idx.x == limit_chars->x)) {
            char_idx.x = 0;
            char_idx.y++;

            if (char_idx.y == limit_chars->y) {
                return;
            }

            char_pos_px.x = pos_px->x;
            char_pos_px.y += char_shift.y;
        } else {
            char_pos_px.x += char_shift.x;
        }
    }
}
