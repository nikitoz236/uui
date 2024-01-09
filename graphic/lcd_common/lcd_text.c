#include "lcd_text.h"
#include "misc.h"

void lcd_text_calc_size(xy_t * size, lcd_text_cfg_t * cfg)
{
    for (int i = 0; i < 2; i++) {
        size->ca[i] =
            (cfg->font->size.ca[i] * cfg->text_size.ca[i] * cfg->scale) +
            ((cfg-> text_size.ca[i] - 1) * cfg->gaps.ca[i])
        ;
    }
}

void lcd_text_extend_text_size(xy_t * available_size, lcd_text_cfg_t * cfg)
{
    for (int i = 0; i < 2; i++) {
        unsigned gap = cfg->gaps.ca[i];
        cfg->text_size.ca[i] = (available_size->ca[i] + gap) / ((cfg->font->size.ca[i] * cfg->scale) + gap);
    }
}

void lcd_text_extend_scale(xy_t * available_size, lcd_text_cfg_t * cfg)
{
    unsigned max_scale = -1;

    for (int i = 0; i < 2; i++) {
        unsigned ts = cfg->text_size.ca[i];
        unsigned gap = DIV_ROUND_UP(cfg->gaps.ca[i], cfg->scale);
        unsigned scale_tmp = available_size->ca[i] / (((ts - 1) * cfg->gaps.ca[i]) + (ts * cfg->font->size.ca[i]));
        if (max_scale > scale_tmp) {
            max_scale = scale_tmp;
        }
    }
    cfg->gaps.w = cfg->gaps.w * max_scale / cfg->scale;
    cfg->gaps.h = cfg->gaps.h * max_scale / cfg->scale;
    cfg->scale = max_scale;
}
