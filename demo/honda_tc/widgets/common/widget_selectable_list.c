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

        // printf("recalc_list item size %d %d\n", item->f.s.w, item->f.s.h);

        if (f.s.h < item->f.s.h) {
            break;
        }

        item->f.p = f.p;
        item->f.s.w = f.s.w;
        form_cut(&f, DIMENSION_Y, EDGE_U, item->f.s.h);

        // printf("recalc_list form %d %d %d %d\n", f.p.x, f.p.y, f.s.w, f.s.h);

        child_idx++;
        ctx->count++;

        if (child_idx >= cfg->num) {
            break;
        }
    }
}

static void redraw_list(ui_element_t * el)
{
    __widget_selectable_list_ctx_t * ctx = (__widget_selectable_list_ctx_t *)el->ctx;
    draw_color_form(&el->f, 0);

    ui_element_t * item = ui_tree_child(el);
    for (unsigned i = 0; i < ctx->count; i++) {
        if (el->active) {
            if (i == ctx->pos) {
                item->active = 1;
            }
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
    ctx->pos = 0;
    recalc_list(el);
    redraw_list(el);
}

static void select(ui_element_t * el, unsigned select)
{
    __widget_selectable_list_ctx_t * ctx = (__widget_selectable_list_ctx_t *)el->ctx;
    el->active = select;
    ui_element_t * item = ui_tree_child_idx(el, ctx->pos);
    ui_tree_element_select(item, select);
}

static void change_selected_cild(ui_element_t * el, unsigned unselect, unsigned select)
{
    ui_element_t * item = ui_tree_child_idx(el, unselect);
    ui_tree_element_select(item, 0);
    item = ui_tree_child_idx(el, select);
    ui_tree_element_select(item, 1);
}

static void move_list(ui_element_t * el)
{
    ui_tree_delete_childs(el);
    recalc_list(el);
    redraw_list(el);
}

static unsigned process_event(ui_element_t * el, unsigned event)
{
    __widget_selectable_list_cfg_t * cfg = (__widget_selectable_list_cfg_t *)el->ui_node->cfg;
    __widget_selectable_list_ctx_t * ctx = (__widget_selectable_list_ctx_t *)el->ctx;

    ui_element_t * item = ui_tree_child_idx(el, ctx->pos);

    if (event == EVENT_BTN_DOWN) {
        unsigned old_pos = ctx->pos;
        if (ctx->pos < ctx->count - 1) {
            // выделение не на самом нижнем элементе, можно опустить не сдвигая список
            ctx->pos++;
            change_selected_cild(el, old_pos, ctx->pos);
        } else {
            // выделение на самом нижнем пункте
            if (ctx->count == cfg->num) {
                // список помещается полностью, двигаеть не нужно, выделяем первый элемент
                ctx->pos = 0;
                change_selected_cild(el, old_pos, ctx->pos);
            } else {
                if ((ctx->first + ctx->count) < cfg->num) {
                    // список можно поднять
                    ctx->first++;
                    move_list(el);
                } else {
                    // список уже в самом конце, перерисовываем весь c верхнего элемента
                    ctx->first = 0;
                    ctx->pos = 0;
                    move_list(el);
                }
            }

        }
        return 1;
    }

    if (event == EVENT_BTN_UP) {
        unsigned old_pos = ctx->pos;
        if (ctx->pos > 0) {
            // выделение не на самом верхнем элементе, можно поднять не сдвигая список
            ctx->pos--;
            change_selected_cild(el, old_pos, ctx->pos);
        } else {
            // выделение на самом верхнем пункте
            if (ctx->count == cfg->num) {
                // список помещается полностью, двигаеть не нужно, выделяем последний элемент
                ctx->pos = ctx->count - 1;
                change_selected_cild(el, old_pos, ctx->pos);
            } else {
                if (ctx->first > 0) {
                    // список можно опустить
                    ctx->first--;
                    move_list(el);
                } else {
                    // список уже в самом начале, перерисовываем весь список так, чтобы был последний элемент
                    // но так как мы не знаем сколько элементов влезет выше него то выводим только один последний элемент
                    ctx->first = cfg->num - 1;
                    ctx ->pos = 0;
                    move_list(el);
                }
            }
        }
        return 1;
    }

    return 0;
}

const widget_desc_t __widget_selectable_list = {
    .calc = 0,
    .draw = draw,
    .update = 0,
    .select = select,
    .process_event = process_event,
    .ctx_size = sizeof(__widget_selectable_list_ctx_t)
};
