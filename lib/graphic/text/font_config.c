#include "font_config.h"

unsigned fcfg_scale(const lcd_font_cfg_t * cfg)
{
    unsigned scale = cfg->scale;
    if (scale) {
        return scale;
    }
    return 1;
}

unsigned fcfg_gap(const lcd_font_cfg_t * cfg, dimension_t d)
{
    unsigned gap = cfg->gaps.ca[d];
    if (gap) {
        return gap;
    }
    return 1;
}

xy_t fcfg_text_size_px(const lcd_font_cfg_t * cfg, xy_t text_size_chars)
{
    xy_t size;
    unsigned scale = fcfg_scale(cfg);

    for (int i = 0; i < DIMENSION_COUNT; i++) {
        unsigned text_len = text_size_chars.ca[i];
        size.ca[i] =
            (cfg->font->size.ca[i] * text_len * scale) +
            ((text_len - 1) * fcfg_gap(cfg, i))
        ;
    }
    return size;
}

xy_t fcfg_text_char_places(const lcd_font_cfg_t * cfg, xy_t size_px)
{
    xy_t text_size_chars;
    unsigned scale = fcfg_scale(cfg);

    for (int i = 0; i < DIMENSION_COUNT; i++) {
        unsigned gap = fcfg_gap(cfg, i);
        text_size_chars.ca[i] = (size_px.ca[i] + gap) / ((cfg->font->size.ca[i] * scale) + gap);
    }

    return text_size_chars;
}
