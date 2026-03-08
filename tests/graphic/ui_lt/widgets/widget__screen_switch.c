#include "widget__screen_switch.h"
#include "event_list.h"
#include <stdio.h>

typedef struct {
    unsigned selector;
} ctx_t;

static ctx_t * ctx(ui_element_t * el)
{
    return (ctx_t *)el->ctx;
}

static void draw(ui_element_t * el)
{
    const widget__screen_switch_cfg_t * cfg = ui_node_desc(el)->cfg;

    printf("screen_switch draw: p=(%d,%d) s=(%d,%d)\n",
        el->f.p.x, el->f.p.y, el->f.s.w, el->f.s.h);

    ui_delete_childs(el);
    ui_element_t * item = ui_add(el, el->f, &cfg->screens_list[ctx(el)->selector]);
    ui_element_draw(item);
}

static unsigned process(ui_element_t * el, ui_event_t event)
{
    const widget__screen_switch_cfg_t * cfg = ui_node_desc(el)->cfg;

    if (event == EVENT_BTN_DOWN) {
        ctx(el)->selector = (ctx(el)->selector + 1) % cfg->screens_num;
        ui_element_draw(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        if (ctx(el)->selector == 0) {
            ctx(el)->selector = cfg->screens_num - 1;
        } else {
            ctx(el)->selector = ctx(el)->selector - 1;
        }
        ui_element_draw(el);
        return 1;
    }
    if (event == EVENT_BTN_OK) {
        ui_element_t * child = ui_child(el);
        if (child) {
            ui_select(child, 1);
            return 1;
        }
    }
    if (event == EVENT_BTN_LEFT) {
        ui_element_t * child = ui_child(el);
        if (child) {
            ui_select(child, 0);
            return 1;
        }
    }
    return 0;
}

const widget_desc_t widget__screen_switch = {
    .ctx_size = WIDGET_CTX_SIZE(ctx_t),
    .draw = draw,
    .process = process,
};
