#include "widget_text.h"
#include "draw_color.h"

typedef struct {
    tf_ctx_t tf;
} ctx_t;

static void redraw(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;
    lcd_color_t bg = cfg->bg[el->active];

    // printf ("redraw bg %06X\n", bg);

    label_color_t * label = cfg->label;
    if (cfg->label_array) {
        label = &cfg->label[el->idx];
    }

    draw_color_form(&el->f, bg);
    label_color(&ctx->tf, cfg->label, bg, el->idx, 0, 0);
}

static void draw(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;

    const tf_cfg_t * tf_cfg = cfg->tf_cfg;
    if (cfg->tf_array) {
        tf_cfg = &cfg->tf_cfg[el->idx];
    }

    if (tf_ctx_calc(&ctx->tf, &el->f, tf_cfg)) {
        // printf("calculated form %d %d %d %d, text %d %d, pos %d %d\n", el->f.s.x, el->f.s.y, el->f.p.x, el->f.p.y, ctx->tf.size.x, ctx->tf.size.y, ctx->tf.xy.x, ctx->tf.xy.y);
        redraw(el);
        el->drawed = 1;
    }
}

const widget_desc_t widget_text = {
    .draw = draw,
    .select = redraw,
    .ctx_size = sizeof(ctx_t)
};
