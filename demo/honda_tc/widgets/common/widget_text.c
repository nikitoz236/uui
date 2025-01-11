#include "widget_text.h"
#include "lcd_text.h"
#include "lcd_text_color.h"
#include "forms_align.h"
#include "str_utils.h"
#include "draw_color.h"

typedef struct {
    xy_t pos;
} ctx_t;

static void draw_widget_text(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;
    color_scheme_t * cs = cfg->cs_unselected;
    if (el->active) {
        cs = cfg->cs_selected;
    }

    // draw_color_form(&el->f, cs.bg);
    xy_t limit = lcd_text_char_places(cfg->fcgf, el->f.s);
    lcd_color_text_raw_print(cfg->text, cfg->fcgf, cs, &ctx->pos, &limit, 0, 0);
}

static void calc(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    el->f.s = size_add_padding(lcd_text_size_px(cfg->fcgf, (xy_t){}), cfg->padding);
}

static void extend(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->pos = align_form_pos(&el->f, lcd_text_size_px(cfg->fcgf, (xy_t){}), ALIGN_LIC, cfg->padding);
}

const widget_desc_t __widget_text = {
    .calc = calc,
    .extend = extend,
    .draw = draw_widget_text,
    .select = draw_widget_text,
    .ctx_size = sizeof(ctx_t)
};
