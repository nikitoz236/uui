#include "widget_union.h"

static unsigned calc(ui_ctx_t * node_ctx) {
    widget_union_cfg_t * union_cfg = (widget_union_cfg_t *)node_ctx->node->widget_cfg;
    ui_ctx_t * child_ctx[2];
    node_ctx->child_offset = 0;
    void * ctx_ptr = node_ctx->ctx;
    unsigned result_size = 0;

    for (int i = 0; i < 2; i++) {
        child_ctx[i] = ctx_ptr;
        unsigned child_node_size = calc_node(&union_cfg->nodes[i], child_ctx[i], i);
        result_size += child_node_size;
        ctx_ptr += child_node_size;
    }

    form_union_calc_size(&child_ctx[0]->f, &child_ctx[1]->f, &union_cfg->align_mode, &node_ctx->f);

    // если режим выравнивания по центру, то надо меньшую форму растянуть до большей
    for (int i = 0; i < 2; i++) {
        if (union_cfg->align_mode.ca[i].mode == ALIGN_CENTER) {
            if (child_ctx[0]->f.s.ca[i] > child_ctx[1]->f.s.ca[i]) {
                child_ctx[1]->f.s.ca[i] = child_ctx[0]->f.s.ca[i];
            } else {
                child_ctx[0]->f.s.ca[i] = child_ctx[1]->f.s.ca[i];
            }
        }
    }

    return result_size;
};

static void draw(ui_ctx_t * node_ctx) {
    widget_union_cfg_t * union_cfg = (widget_union_cfg_t *)node_ctx->node->widget_cfg;

    // получаем указатели на контексты дочерних форм
    ui_ctx_t * child_ctx[2];
    void * ctx_ptr = node_ctx->ctx;
    for (int i = 0; i < 2; i++) {
        child_ctx[i] = ctx_ptr;
        unsigned child_node_size = child_ctx[i]->size;
        ctx_ptr += child_node_size;
    }

    form_union_calc_pos(&node_ctx->f, &child_ctx[0]->f, &child_ctx[1]->f, &union_cfg->align_mode);
    for (int i = 0; i < 2; i++) {
        unsigned child_idx = child_ctx[i]->idx;
        draw_node(child_ctx[i]);
    }
};

widget_t __widget_union = {
    .calc = calc,
    .draw = draw
};
