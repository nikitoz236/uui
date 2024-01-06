#include "widget_emu_button.h"
#include "emu_common.h"

static unsigned calc(ui_ctx_t * node_ctx)
{
    widget_emu_button_cfg_t * cfg = (widget_emu_button_cfg_t *)node_ctx->node->widget_cfg;
    widget_emu_button_ctx_t * ctx = (widget_emu_button_ctx_t*)node_ctx->ctx;
    ctx->state = 0;
    node_ctx->f.s = cfg->size;
    return 0;
};

static void draw(ui_ctx_t * node_ctx)
{
    widget_emu_button_cfg_t * cfg = (widget_emu_button_cfg_t *)node_ctx->node->widget_cfg;
    widget_emu_button_ctx_t * ctx = (widget_emu_button_ctx_t *)node_ctx->ctx;
    __emu_button_index_cfg_t * index_cfg = &cfg->index_cfg[node_ctx->idx];

    printf("widget_emu_button_cfg_t size %d x: %d y: %d icfg: %p, char: %c\r\n", sizeof(widget_emu_button_cfg_t), node_ctx->f.p.x, node_ctx->f.p.y, index_cfg, index_cfg->key);

    unsigned state = ctx->state;

    if (state == 0) {
        emu_draw_rect(&node_ctx->f, cfg->color_released);
    } else {
        emu_draw_rect(&node_ctx->f, cfg->color_pressed);
    }
    unsigned x = node_ctx->f.p.x + (node_ctx->f.s.w / 2);
    unsigned y = node_ctx->f.p.y + (node_ctx->f.s.h / 2);
    emu_set_color(cfg->color_text);
    gfx_text(x, y, &index_cfg->key, 1);
};

static unsigned process(ui_ctx_t * node_ctx, unsigned event)
{
    widget_emu_button_cfg_t * cfg = (widget_emu_button_cfg_t *)node_ctx->node->widget_cfg;
    widget_emu_button_ctx_t * ctx = (widget_emu_button_ctx_t *)node_ctx->ctx;
    __emu_button_index_cfg_t * index_cfg = &cfg->index_cfg[node_ctx->idx];

    if (event == index_cfg->key) {
        ctx->state = !ctx->state;
        draw(node_ctx);
        return 1;
    }
    return 0;
}

widget_t __widget_emu_button = {
    .calc = calc,
    .draw = draw,
    .process = process,
    .ctx_len = sizeof(widget_emu_button_ctx_t),
};
