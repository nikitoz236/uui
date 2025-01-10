#include "widget_metric_list.h"
#include "draw_color.h"
#include "event_list.h"
#include "align_forms.h"

typedef struct {
    uint8_t pos;
    uint8_t max;
} __widget_metric_list_ctx_t;

static void list_update(form_t * f, xy_t * borders, xy_t * gaps, ui_element_t * item, unsigned max, unsigned pos)
{
    xy_t grid_size = { 1, max };
    for (unsigned i = 0; i < max; i++) {
        form_grid(f, &item->f, borders, gaps, &grid_size, 0, i);
        item->idx = i + pos;
        ui_tree_element_update(item);
    }
}

static void list_draw(form_t * f, xy_t * borders, xy_t * gaps, ui_element_t * item, unsigned max, unsigned pos)
{
    xy_t grid_size = { 1, max };
    for (unsigned i = 0; i < max; i++) {
        form_grid(f, &item->f, borders, gaps, &grid_size, 0, i);
        item->idx = i + pos;
        ui_tree_element_draw(item);
    }
}

static void update(ui_element_t * el)
{
    __widget_metric_list_cfg_t * cfg = (__widget_metric_list_cfg_t *)el->ui_node->cfg;
    __widget_metric_list_ctx_t * ctx = (__widget_metric_list_ctx_t *)el->ctx;

    ui_element_t * item = ui_tree_child(el);
    list_update(&el->f, &cfg->borders, &cfg->gaps, item, ctx->max, ctx->pos);
}

static void draw(ui_element_t * el)
{
    __widget_metric_list_cfg_t * cfg = (__widget_metric_list_cfg_t *)el->ui_node->cfg;
    __widget_metric_list_ctx_t * ctx = (__widget_metric_list_ctx_t *)el->ctx;

    draw_color_form(&el->f, cfg->color_bg);

    ui_element_t * item = ui_tree_add(el, cfg->ui_node, 0);
    ui_tree_element_calc(item);

    ctx->pos = 0;
    ctx->max = (el->f.s.h + cfg->gaps.y - (2 * cfg->borders.h)) / (item->f.s.h + cfg->gaps.y);
    item->f.s.w = el->f.s.w - (2 * cfg->borders.w);

    list_draw(&el->f, &cfg->borders, &cfg->gaps, item, ctx->max, ctx->pos);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    __widget_metric_list_cfg_t * cfg = (__widget_metric_list_cfg_t *)el->ui_node->cfg;
    __widget_metric_list_ctx_t * ctx = (__widget_metric_list_ctx_t *)el->ctx;
    ui_element_t * item = ui_tree_child(el);
    unsigned refresh = 0;
    if (event == EVENT_BTN_UP) {
        if (ctx->pos > 0) {
            ctx->pos--;
            refresh = 1;
        }
    } else if (event == EVENT_BTN_DOWN) {
        if (ctx->pos < (cfg->num - ctx->max)) {
            ctx->pos++;
            refresh = 1;
        }
    } else {
        return 0;
    }
    if (refresh) {
        list_draw(&el->f, &cfg->borders, &cfg->gaps, item, ctx->max, ctx->pos);
        list_update(&el->f, &cfg->borders, &cfg->gaps, item, ctx->max, ctx->pos);
    }
    return 1;
}

widget_desc_t __widget_metric_list = {
    .draw = draw,
    .update = update,
    .process_event = process,
    .ctx_size = sizeof(__widget_metric_list_ctx_t)
};
