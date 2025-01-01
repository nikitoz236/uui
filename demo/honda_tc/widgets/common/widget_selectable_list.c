#include "widget_selectable_list.h"
#include "align_forms.h"
#include "forms_split.h"
#include "tc_events.h"
#include "draw_color.h"

typedef struct {
    uint8_t pos;
    uint8_t first;
    uint8_t count;
} __widget_selectable_list_ctx_t;

static void recalc_list(ui_element_t * el)
{
    __widget_selectable_list_cfg_t * cfg = (__widget_selectable_list_cfg_t *)el->ui_node->cfg;
    __widget_selectable_list_ctx_t * ctx = (__widget_selectable_list_ctx_t *)el->ctx;

    ctx->count = 0;
    unsigned child_idx = ctx->first;
    form_t f = el->f;

    while (1) {

        ui_element_t * item = ui_tree_add(el, &cfg->ui_node[child_idx], child_idx);
        ui_tree_element_calc(item);

        if (f.s.h < item->f.s.h) {
            break;
        }

        item->f.p = f.p;
        item->f.s.w = f.s.w;
        form_cut(&f, DIMENSION_Y, EDGE_U, item->f.s.h);

        child_idx++;
        ctx->count++;

        if (child_idx >= cfg->num) {
            break;
        }
    }
}

void redraw_list(ui_element_t * el)
{
    __widget_selectable_list_ctx_t * ctx = (__widget_selectable_list_ctx_t *)el->ctx;
    draw_color_form(&el->f, 0);

    ui_element_t * item = ui_tree_child(el);
    for (unsigned i = 0; i < ctx->count; i++) {
        if (i == ctx->pos) {
            item->active = 1;
        }
        ui_tree_element_draw(item);
        item = ui_tree_next(item);
    }
}

static void draw(ui_element_t * el)
{
    __widget_selectable_list_cfg_t * cfg = (__widget_selectable_list_cfg_t *)el->ui_node->cfg;
    __widget_selectable_list_ctx_t * ctx = (__widget_selectable_list_ctx_t *)el->ctx;

    ctx->first = 0;
    ctx->pos = 5;
    recalc_list(el);
    redraw_list(el);
}

ui_element_t * ui_get_child(ui_element_t * el, unsigned idx)
{
    ui_element_t * item = ui_tree_child(el);
    for (unsigned i = 0; i < idx; i++) {
        item = ui_tree_next(item);
    }
    return item;
}

unsigned process_event(ui_element_t * el, unsigned event)
{
    __widget_selectable_list_cfg_t * cfg = (__widget_selectable_list_cfg_t *)el->ui_node->cfg;
    __widget_selectable_list_ctx_t * ctx = (__widget_selectable_list_ctx_t *)el->ctx;

    if (event == EVENT_BTN_DOWN) {
        if (ctx->pos < ctx->count - 1) {
            ui_element_t * item = ui_get_child(el, ctx->pos);
            ui_tree_element_select(item, 0);
            item = ui_tree_next(item);
            ui_tree_element_select(item, 1);
            ctx->pos++;
            return 1;
        } else {
            if ((ctx->first + ctx->count) < cfg->num) {
                ctx->first++;
                ui_tree_delete_childs(el);
                recalc_list(el);
                redraw_list(el);
                return 1;
            }
        }
    } else if (event == EVENT_BTN_UP) {
        if (ctx->pos > 0) {
            ctx->pos--;
            ui_element_t * item = ui_get_child(el, ctx->pos);
            ui_tree_element_select(item, 1);
            item = ui_tree_next(item);
            ui_tree_element_select(item, 0);
            return 1;
        } else {
            if (ctx->first > 0) {
                ctx->first--;
                ui_tree_delete_childs(el);
                recalc_list(el);
                redraw_list(el);
                return 1;
            }
        }
    }

    return 0;
}

const widget_desc_t __widget_selectable_list = {
    .calc = 0,
    .draw = draw,
    .update = 0,
    .select = 0,
    .process_event = process_event,
    .ctx_size = sizeof(__widget_selectable_list_ctx_t)
};
