#include "widget_grid.h"

void form_grid_calc_size(xy_t * item_size, xy_t * result_size, form_grid_t * grid_cfg)
{
    for (int i = 0; i < 2; i++) {
        unsigned gap = grid_cfg->ca[i].gap;
        result_size->ca[i] = ((item_size->ca[i] + gap) * grid_cfg->ca[i].count) - gap;
    }
}

void from_grid_place(form_t * box, form_t * item, form_grid_t * grid_cfg, unsigned index, unsigned stretch)
{
    unsigned indexes[2] = {
        index % grid_cfg->ca[0].count,
        index / grid_cfg->ca[0].count
    };

    printf("        index: %d, indexes: (%d, %d), stretch: %d, ", index, indexes[0], indexes[1], stretch);
    form_t tmp_form;
    for (int i = 0; i < 2; i++) {
        unsigned gap = grid_cfg->ca[i].gap;
        unsigned size = ((box->s.ca[i] + gap) / grid_cfg->ca[i].count) - gap;
        unsigned pos = box->p.ca[i] + (size + gap) * indexes[i];
        tmp_form.s.ca[i] = size;
        tmp_form.p.ca[i] = pos;
    }
    printf("pos: (%d, %d), size: (%d, %d)\r\n", tmp_form.p.x, tmp_form.p.y, tmp_form.s.w, tmp_form.s.h);
    if (stretch) {
        *item = tmp_form;
    } else {
        form_align(&tmp_form, item, &ALIGN_MODE(C, 0, C, 0));
    }
}

static unsigned calc(ui_ctx_t * node_ctx)
{
    widget_grid_cfg_t * cfg = (widget_grid_cfg_t *)node_ctx->node->widget_cfg;
    unsigned elements_count = cfg->grid_cfg.ca[0].count * cfg->grid_cfg.ca[1].count;
    ui_ctx_t * child_ctx = first_child(node_ctx);
    unsigned child_node_type = 0;
    unsigned child_ctx_size = 0;
    unsigned child_node_count = 0;
    xy_t max_size = {0, 0};

    for (int i = 0; i < elements_count; i++) {
        child_ctx_size += calc_node(cfg->childs[child_node_type].node, child_ctx, i);
        for (int j = 0; j < 2; j++) {
            if (max_size.ca[j] < child_ctx->f.s.ca[j]) {
                max_size.ca[j] = child_ctx->f.s.ca[j];
            }
        }
        unsigned st = child_ctx->node->widget->stretched;
        printf("calc grid item %d size: (%d, %d), max size: (%d, %d), stretch %d\r\n", i, child_ctx->f.s.w, child_ctx->f.s.h, max_size.w, max_size.h, st);

        child_ctx = next_child(child_ctx);
        child_node_count++;
        if (child_node_count == cfg->childs[child_node_type].count) {
            child_node_type++;
            child_node_count = 0;
        }
    }

    form_grid_calc_size(&max_size, &node_ctx->f.s, &cfg->grid_cfg);

    // в конфиге лежит описатель ноды. вот нам надо получить его размер
    // дальше по двум измерениям пройтись и посчитать размер всего грида
    // надо решить будут ли нижние ноды чтото хранить в своем контексте
    // либо мы можем один контекст динамически пересчитывать под все элементы когда будем их отрисовывать

    return child_ctx_size;
};

static void draw(ui_ctx_t * node_ctx)
{
    widget_grid_cfg_t * cfg = (widget_grid_cfg_t *)node_ctx->node->widget_cfg;
    unsigned elements_count = cfg->grid_cfg.ca[0].count * cfg->grid_cfg.ca[1].count;
    ui_ctx_t * child_ctx = first_child(node_ctx);

    for (int i = 0; i < elements_count; i++) {
        from_grid_place(&node_ctx->f, &child_ctx->f, &cfg->grid_cfg, i, child_ctx->node->widget->stretched);
        printf("draw grid item %d pos: (%d, %d), size: (%d, %d)\r\n", i, child_ctx->f.p.x, child_ctx->f.p.y, child_ctx->f.s.w, child_ctx->f.s.h);
        draw_node(child_ctx);
        child_ctx = next_child(child_ctx);
    }
}

widget_t __widget_grid = {
    .calc = calc,
    .draw = draw,
    .stretched = 1
};
