#include "widget_text.h"
#include "lcd_text.h"
#include "lcd_text_color.h"
#include "forms_align.h"
#include "str_utils.h"
#include "draw_color.h"

typedef struct {
    xy_t pos;
    lcd_text_cfg_t text_cfg;
} widget_text_ctx_t;

static void draw_widget_text(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    widget_text_ctx_t * ctx = (widget_text_ctx_t *)el->ctx;

    color_scheme_t cs = { .fg = cfg->color_text };
    if (el->active) {
        cs.bg = cfg->color_bg_selected;
        // printf("draw selected text %s\n", cfg->text);
    } else {
        cs.bg = cfg->color_bg_unselected;
        // printf("draw unselected text %s\n", cfg->text);
    }
    draw_color_form(&el->f, cs.bg);
    lcd_text_color_print(cfg->text, &ctx->pos, &ctx->text_cfg, &cs, 0, 0, 0);
}

static void calc(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    widget_text_ctx_t * ctx = (widget_text_ctx_t *)el->ctx;

    ctx->text_cfg.font = cfg->font;
    ctx->text_cfg.text_size = (xy_t){ .x = str_len(cfg->text, 20), .y = 1};
    ctx->text_cfg.gaps = (xy_t){.x = cfg->scale, .y = cfg->scale};
    ctx->text_cfg.scale = cfg->scale;

    lcd_text_calc_size(&el->f.s, &ctx->text_cfg);

    el->f.s.h += 2 * cfg->scale;        // зазоры по вертикали
}

static void draw(ui_element_t * el)
{
    widget_text_ctx_t * ctx = (widget_text_ctx_t *)el->ctx;

    // lcd_text_extend_scale(&el->f.s, &ctx->text_cfg);

    form_t text_form;
    lcd_text_calc_size(&text_form.s, &ctx->text_cfg);
    align_form(&el->f, &text_form, &(align_t){ .x = { .edge = EDGE_L }, .y = { .center = 1 }}, &(xy_t){.x = 5, .y = 0});
    ctx->pos = text_form.p;

    draw_widget_text(el);
}

const widget_desc_t __widget_text = {
    .calc = calc,
    .draw = draw,
    .select = draw_widget_text,
    .ctx_size = sizeof(widget_text_ctx_t)
};
