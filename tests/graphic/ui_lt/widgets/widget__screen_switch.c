#include "widget__screen_switch.h"
#include "event_list.h"
#include "val_mod.h"
#include <stdio.h>
#include "dp.h"

typedef struct {
    uint8_t selector;
} ctx_t;

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    const widget__screen_switch_cfg_t * cfg = ui_node_desc(el)->cfg;

    ui_element_t * item = ui_add(el, el->f, &cfg->screens_list[ctx->selector]);
    ui_element_draw(item);
}

static unsigned process(ui_element_t * el, ui_event_t event)
{
    dpn("process widget__screen_switch");
    ctx_t * ctx = (ctx_t *)el->ctx;
    const widget__screen_switch_cfg_t * cfg = ui_node_desc(el)->cfg;

    unsigned p = 0;
    val_mod_op_t op;

    if (event == EVENT_BTN_DOWN) {
        p = 1;
        op = MOD_OP_SUB;
    }

    if (event == EVENT_BTN_UP) {
        p = 1;
        op = MOD_OP_ADD;
    }

    if (p) {
        if (val_mod_unsigned(&ctx->selector, VAL_SIZE_8, op, 1, 0, cfg->count - 1, 1)) {
            ui_delete_childs(el);
            draw(el);
        }
    }

    return p;
}

const widget_desc_t widget__screen_switch = {
    .ctx_size = WIDGET_CTX_SIZE(ctx_t),
    .draw = draw,
    .process_event = process,
};
