#include "widget_selectable_list.h"
#include "align_forms.h"
#include "forms_split.h"
#include "event_list.h"
#include "draw_color.h"

/*
    нужно добавить движение все списка кнопками для варианта selectable = 0
    еще нужно обрабатывать cycled
*/

typedef struct {
    uint8_t pos;        // номер выбранного элемента среди тех что на экране
    uint8_t first;      // номер элемента в списке с которого начинается отрисовка списка
    uint8_t count;      // количество элементов поместившихся на экране
} ctx_t;

static void recalc(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_selectable_list_cfg_t * cfg = (widget_selectable_list_cfg_t *)el->ui_node->cfg;
    draw_color_form(&el->f, 0);

    ctx->count = 0;
    form_t f = el->f;
    form_cut_padding(&f, cfg->margin);
    unsigned child_idx = ctx->first;

    const ui_node_desc_t * child_ui_node = &cfg->ui_node[0];

    while (child_idx < cfg->num) {
        if (cfg->different_nodes) {
            child_ui_node = &cfg->ui_node[child_idx];
        }

        ui_element_t * item = ui_tree_add(el, child_ui_node, child_idx);
        item->f = f;

        if (el->active) {
            if (ctx->count == ctx->pos) {
                item->active = 1;
            }
        }

        ui_tree_element_draw(item);

        if (item->drawed == 0) {
            break;
        }

        form_cut(&f, DIMENSION_Y, EDGE_U, item->f.s.h);
        child_idx++;
        ctx->count++;

        if (f.s.h < cfg->margin.y) {
            break;
        }

        form_cut(&f, DIMENSION_Y, EDGE_U, cfg->margin.y);
    }

    // printf("widget_selectable_list recalc, count %d, first %d, pos %d\n", ctx->count, ctx->first, ctx->pos);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->first = 0;
    ctx->pos = 0;
    recalc(el);
    el->drawed = 1;
}

static void select(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ui_element_t * item = ui_tree_child_idx(el, ctx->pos);
    // printf("widget_selectable_list select, active %d, pos %d\n", el->active, ctx->pos);
    ui_tree_element_select(item, el->active);
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
    recalc(el);
}

// static void move_selector_dn(ctx_t * ctx, unsigned num)
// {

// }

// static void move_selector_up(ctx_t * ctx, unsigned num)
// {

// }

// static void move_list_dn(void)
// {

// }

// static void move_list_up(void)
// {

// }


static unsigned process_event(ui_element_t * el, unsigned event)
{
    widget_selectable_list_cfg_t * cfg = (widget_selectable_list_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;

    // printf("widget_selectable_list process_event active %d, event %d\n", el->active, event);

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

const widget_desc_t widget_selectable_list = {
    .draw = draw,
    .select = select,
    .process_event = process_event,
    .ctx_size = sizeof(ctx_t)
};
