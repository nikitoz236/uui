#include "widget_text_color.h"

static unsigned calc(void * widget_cfg, ui_ctx_t * node_ctx)
{
    widget_text_color_cfg_t * cfg = (widget_text_color_cfg_t *)widget_cfg;
    for (int i = 0; i < 2; i++) {
        node_ctx->f.s.ca[i] =
            (cfg->font->size.ca[i] * cfg-> text_size.ca[i]) +
            ((cfg-> text_size.ca[i] - 1) + cfg->gaps.ca[i])
        ;
    }
    return sizeof(ui_ctx_t);
};

static void draw(void * widget_cfg, void * widget_icfg, ui_ctx_t * node_ctx) {
    widget_text_color_cfg_t * cfg = (widget_text_color_cfg_t *)widget_cfg;
    widget_text_color_index_cfg_t * icfg = (widget_text_color_index_cfg_t *)widget_icfg;

    unsigned y = node_ctx->f.p.y;
    for (int cy = 0; cy < cfg->text_size.h; cy++) {
        unsigned x = node_ctx->f.p.x;
        if (cy != (cfg->text_size.h - 1)) {
            lcd_rect(x, y, (cfg->text_size.w * cfg->font->size.w) + ((cfg->text_size.w - 1) * cfg->gaps.w), cfg->gaps.h, icfg->cs->bg);
        }
        for(int cx = 0; cx < cfg->text_size.w ; cx++) {
            lcd_rect(x, y, cfg->font->size.w, cfg->font->size.h, icfg->cs->fg);
            x += cfg->font->size.w;

            if (cx != (cfg->text_size.w - 1)) {
                lcd_rect(x, y, cfg->gaps.w, cfg->font->size.h, icfg->cs->bg);
            }
            x += cfg->gaps.w;
        }
        y += cfg->font->size.h + cfg->gaps.h;
    }
};

widget_t __widget_text_color = {
    .calc = calc,
    .draw = draw
};
