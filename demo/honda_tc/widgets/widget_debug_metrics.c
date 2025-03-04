#include "ui_tree.h"

#include "text_label_color.h"
#include "lcd_color.h"
#include "draw_color.h"
#include "val_mod.h"

#include "event_list.h"

#include "widget_debug_metrics.h"
#include "widget_metric_list.h"
#include "widget_metric_list_item.h"

// #include "widget_titled_screen.h"
// #include "metrics_view.h"

typedef struct {
    form_t title_form;
    tf_ctx_t title_ctx;
    unsigned selector;
} ctx_t;


/*
============================================
0 2         13
< METRICS   >

============================================

*/

const lp_color_t title = {
    .color = 0xFFB62E, .l = { .xy = { .x = 2 }, .len = 10, .text = (const char * []){ "METRICS", "DLC DUMP" }, .t = LP_T_LIDX }
};

const lp_color_t local_title = {
    .color = 0xA36A00, .l = { .xy = { .x = -3 }, .len = 4, .text = (const char * []){ "REAL", "BOOL" }, .t = LP_T_LIDX }
};

const label_list_t title_selector = {
    .wrap_list = (lp_color_t []) {
        { .color = 0xFFB62E, .l = { .xy = { .x =  0 }, .len = 1, .text_list = (const char *[]){ "<", 0 }, .t = LP_T_LIDX } },
        { .color = 0xFFB62E, .l = { .xy = { .x = 13 }, .len = 1, .text_list = (const char *[]){ ">", 0 }, .t = LP_T_LIDX } },
        { .color = 0xFFB62E, .l = { .xy = { .x = -8 }, .len = 1, .text_list = (const char *[]){ 0, "<" }, .t = LP_T_LIDX } },
        { .color = 0xFFB62E, .l = { .xy = { .x = -1 }, .len = 1, .text_list = (const char *[]){ 0, ">" }, .t = LP_T_LIDX } },
    },
    .count = 4
};

// const lp_color_t type = {
//     .color = 0xCE4512, .l = { .xy = { .x = 1 }, .len = 20, .text_list = { "REAL", "BOOL" }, .t = LP_T }
// }

static const lcd_color_t bg = 0x4585E1;

static void redraw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_screen_debug_metrics_cfg_t * cfg = (widget_screen_debug_metrics_cfg_t *)el->ui_node->cfg;

    lp(&ctx->title_ctx, &local_title.l, &(color_scheme_t){ .fg = local_title.color, .bg = bg }, 0, 0, ctx->selector);
    lp_color(&ctx->title_ctx, bg, &title_selector, el->active, 0, 0);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_screen_debug_metrics_cfg_t * cfg = (widget_screen_debug_metrics_cfg_t *)el->ui_node->cfg;
    ctx->selector = 0;
    ctx->title_form = el->f;
    tf_ctx_calc(&ctx->title_ctx, &ctx->title_form, cfg->title_cfg);
    draw_color_form(&ctx->title_form, bg);

    lp(&ctx->title_ctx, &title.l, &(color_scheme_t){ .fg = title.color, .bg = bg }, 0, 0, el->idx);

    // ui_element_t * item = ui_tree_add(el, &debug_metrics_screen, 0);
    // ui_tree_element_draw(item);
    redraw(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_screen_debug_metrics_cfg_t * cfg = (widget_screen_debug_metrics_cfg_t *)el->ui_node->cfg;

    if (event == EVENT_BTN_DOWN) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_ADD, 1, 0, 1, 1);
        redraw(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_SUB, 1, 0, 1, 1);
        redraw(el);
        return 1;
    }
    return 0;
}

const widget_desc_t widget_screen_debug_metrics = {
    .draw = draw,
    .select = redraw,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
