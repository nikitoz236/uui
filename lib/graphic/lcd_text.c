#include "lcd_text.h"
#include "misc.h"

void lcd_text_calc_size(xy_t * size, const lcd_text_cfg_t * cfg)
{
    for (int i = 0; i < DIMENSION_COUNT; i++) {
        size->ca[i] =
            (cfg->font->size.ca[i] * cfg->text_size.ca[i] * cfg->scale) +
            ((cfg-> text_size.ca[i] - 1) * cfg->gaps.ca[i])
        ;
    }
}

xy_t lcd_text_size_px(const lcd_font_cfg_t * cfg, xy_t text_size_chars)
{
    xy_t size;

    // printf("text_size_chars %d %d\n", text_size_chars->x, text_size_chars->y);

    unsigned scale = cfg->scale;
    if (scale == 0) {
        scale = 1;
    }

    for (int i = 0; i < DIMENSION_COUNT; i++) {
        unsigned text_len = text_size_chars.ca[i];
        if (text_len == 0) {
            text_len = 1;
        }

        unsigned gap = cfg->gaps.ca[i];
        if (gap == 0) {
            gap = 1;
        }

        size.ca[i] =
            (cfg->font->size.ca[i] * text_len * scale) +
            ((text_len - 1) * gap)
        ;
    }

    // printf("size %d %d\n", size.x, size.y);

    return size;
}

xy_t lcd_text_char_places(const lcd_font_cfg_t * cfg, xy_t size_px)
{
    xy_t text_size_chars;

    unsigned scale = cfg->scale;
    if (scale == 0) {
        scale = 1;
    }

    for (int i = 0; i < DIMENSION_COUNT; i++) {
        unsigned gap = cfg->gaps.ca[i];
        if (gap == 0) {
            gap = 1;
        }

        text_size_chars.ca[i] = (size_px.ca[i] + gap) / ((cfg->font->size.ca[i] * scale) + gap);
    }

    return text_size_chars;
}

void lcd_text_extend_text_size(const xy_t * available_size, lcd_text_cfg_t * cfg)
{
    for (int i = 0; i < 2; i++) {
        unsigned gap = cfg->gaps.ca[i];
        cfg->text_size.ca[i] = (available_size->ca[i] + gap) / ((cfg->font->size.ca[i] * cfg->scale) + gap);
    }
}

void lcd_text_extend_scale(const xy_t * available_size, lcd_text_cfg_t * cfg)
{
    unsigned max_scale = -1;

    for (int i = 0; i < DIMENSION_COUNT; i++) {
        unsigned ts = cfg->text_size.ca[i];

        // if (cfg->gaps.ca[i] == 0) {
        //     cfg->gaps.ca[i] = 1;
        // }

        unsigned scale_tmp = available_size->ca[i] / (((ts - 1) * cfg->gaps.ca[i]) + (ts * cfg->font->size.ca[i]));
        if (max_scale > scale_tmp) {
            max_scale = scale_tmp;
        }
    }

    // if (cfg->scale == 0) {
    //     cfg->scale = 1;
    // }

    cfg->gaps.w = cfg->gaps.w * max_scale / cfg->scale;
    cfg->gaps.h = cfg->gaps.h * max_scale / cfg->scale;
    cfg->scale = max_scale;
}

void lcd_text_extend_gaps(const xy_t * available_size, lcd_text_cfg_t * cfg)
{
    for (int i = 0; i < DIMENSION_COUNT; i++) {
        unsigned ts = cfg->text_size.ca[i];
        unsigned gap = (available_size->ca[i] - (ts * cfg->font->size.ca[i] * cfg->scale)) / (ts - 1);
        cfg->gaps.ca[i] = gap;
    }
}
