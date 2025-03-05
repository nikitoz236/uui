#include "widget_screen_switch.h"
#include "event_list.h"
#include "forms_split.h"
#include "draw_color.h"
#include "lcd_color.h"
#include "val_mod.h"

typedef struct {
    form_t title_form;
    tf_ctx_t title_ctx;
    unsigned selector;
} ctx_t;

static void redraw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    widget_screen_switch_cfg_t * cfg = (widget_screen_switch_cfg_t *)el->ui_node->cfg;


    ui_node_desc_t * node = &cfg->screens_list[0];
    if (cfg->different_nodes) {
        node = &cfg->screens_list[ctx->selector];
    }
    ui_element_t * item = ui_tree_add(el, node, ctx->selector);

    if (cfg->title_cfg) {
        form_cut(&item->f, DIMENSION_Y, EDGE_U, ctx->title_form.s.h);
        draw_color_form(&ctx->title_form, 0x76ab23);
        lcd_color_tf_print(cfg->titles[ctx->selector], &ctx->title_ctx, &(color_scheme_t){ .bg = 0x76ab23, .fg = 0xAE349E }, 0, 0);
    }

    ui_tree_element_draw(item);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_screen_switch_cfg_t * cfg = (widget_screen_switch_cfg_t *)el->ui_node->cfg;

    ctx->selector = 0;

    if (cfg->title_cfg) {
        ctx->title_form = el->f;
        tf_ctx_calc(&ctx->title_ctx, &ctx->title_form, cfg->title_cfg);
    }

    redraw(el);
}

static void draw_new_child(ui_element_t * el)
{
    ui_tree_delete_childs(el);
    redraw(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    widget_screen_switch_cfg_t * cfg = (widget_screen_switch_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;
    ui_element_t * item = ui_tree_child(el);

    if (event == EVENT_BTN_DOWN) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_ADD, 1, 0, cfg->screens_num - 1, 1);
        draw_new_child(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_SUB, 1, 0, cfg->screens_num - 1, 1);
        draw_new_child(el);
        return 1;
    }
    if (event == EVENT_BTN_OK) {
        return ui_tree_element_select(item, 1);
    }
    if (event == EVENT_BTN_LEFT) {
        return ui_tree_element_select(item, 0);
    }
    return 0;
}

const widget_desc_t widget_screen_switch = {
    .draw = draw,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
