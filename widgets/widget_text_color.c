#include "widget_text_color.h"
#include "api_lcd_color.h"

static unsigned calc(void * widget_cfg, ui_ctx_t * node_ctx)
{
    widget_text_color_cfg_t * cfg = (widget_text_color_cfg_t *)widget_cfg;
    for (int i = 0; i < 2; i++) {
        node_ctx->f.s.ca[i] =
            (cfg->font->size.ca[i] * cfg-> text_size.ca[i] * cfg->scale) +
            ((cfg-> text_size.ca[i] - 1) * cfg->gaps.ca[i])
        ;
    }
    return sizeof(ui_ctx_t);
};

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

static inline void print_char(char c, unsigned x, unsigned y, font_t * font, color_scheme_t * cs, unsigned scale)
{
    // обработает нам одно знакоместо
    uint8_t col_step;
    uint8_t * font_data = font_char_ptr(c, font, &col_step);
    if (font_data != 0) {
        for (int rx = 0; rx < font->size.w; rx++) {
            lcd_color_t * col_buf = char_col_buffer[rx & 1];
            uint8_t mask = 1;
            uint8_t * font_line = font_data;
            for (int ry = 0; ry < font->size.h; ry++) {
                if (*font_data & mask) {
                    col_buf[ry] = cs->fg;
                } else {
                    col_buf[ry] = cs->bg;
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
            col_buf[ry] = cs->fg;
            ry++;
            col_buf[ry] = cs->bg;
            ry++;
        }

        for (int rx = 0; rx < font->size.w; rx++) {
            lcd_image(x + rx, y, 1, font->size.h, scale, &col_buf[rx & 1]);
        }
    }
}

static inline void fill_rem_form_str(unsigned x, unsigned y, unsigned rem_chars, widget_text_color_cfg_t * cfg, unsigned color)
{
    unsigned rem_w = (rem_chars * cfg->font->size.w * cfg->scale) + ((rem_chars - 1) * cfg->gaps.w);
    lcd_rect(x, y, rem_w, cfg->font->size.h * cfg->scale, color);
}

static void draw(void * widget_cfg, void * widget_icfg, ui_ctx_t * node_ctx) {
    widget_text_color_cfg_t * cfg = (widget_text_color_cfg_t *)widget_cfg;
    widget_text_color_index_cfg_t * icfg = (widget_text_color_index_cfg_t *)widget_icfg;

    char * c = icfg->text;

    unsigned y = node_ctx->f.p.y;
    for (int cy = 0; cy < cfg->text_size.h; cy++) {
        unsigned x = node_ctx->f.p.x;
        for(int cx = 0; cx < cfg->text_size.w ; cx++) {
            // если возврат каретки     то игнорируем его               увеличиваем указатель       не выходим из цикла
            // если 0                   то рисуем до конца формы фон    не увеличиваем указатель    выходим из цикла
            // если перевод строки      то рисуем до конца формы фон    увеличиваем указатель       выходим из цикла
            // если пробел              то рисуем до конца символа фон  увеличиваем указатель       не выходим из цикла

            // внутри print_char()
            // если символ в таблице    то рисуем его                   увеличиваем указатель       не выходим из цикла
            // если не в таблице        то рисуем шашечку               увеличиваем указатель       не выходим из цикла
            // utf 8 потом надо бы ....

            if (*c == '\r') {
                c++;
            }

            lcd_color_t bg = icfg->cs->bg;
            if (*c == 0) {
                // bg = 0xFF0000;
                fill_rem_form_str(x, y, cfg->text_size.w - cx, cfg, bg);
                cx = cfg->text_size.w;
            } else if (*c == '\n') {
                // bg = 0xFFFF00;
                fill_rem_form_str(x, y, cfg->text_size.w - cx, cfg, bg);
                cx = cfg->text_size.w;
                c++;
            } else {
                if (*c == ' ') {
                    // bg = 0x00FF00;
                    lcd_rect(x, y, (cfg->font->size.w * cfg->scale) + cfg->gaps.w, (cfg->font->size.h * cfg->scale), bg);
                    x += cfg->font->size.w * cfg->scale + cfg->gaps.w;
                } else {
                    // lcd_rect(x, y, cfg->font->size.w * cfg->scale, cfg->font->size.h * cfg->scale, icfg->cs->fg);
                    print_char(*c, x, y, cfg->font, icfg->cs, cfg->scale);
                    x += cfg->font->size.w * cfg->scale;
                    if (cx != (cfg->text_size.w - 1)) {
                        // bg = 0xFF00FF;
                        lcd_rect(x, y, cfg->gaps.w, cfg->font->size.h * cfg->scale, bg);
                    }
                    x += cfg->gaps.w;
                }
                c++;
            }
        }
        y += cfg->font->size.h * cfg->scale;

        if (cy != (cfg->text_size.h - 1)) {
            lcd_rect(node_ctx->f.p.x, y, node_ctx->f.s.w, cfg->gaps.h, icfg->cs->bg);
        }
        y += cfg->gaps.h;
    }
};

widget_t __widget_text_color = {
    .calc = calc,
    .draw = draw
};
