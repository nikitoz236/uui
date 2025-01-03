#include "widget_template.h"

typedef struct {
    
} widget_template_ctx_t;

static void calc(ui_element_t * el)
{
    widget_template_cfg_t * cfg = (widget_template_cfg_t *)el->ui_node->cfg;
    widget_template_ctx_t * ctx = (widget_template_ctx_t *)el->ctx;
}

static void update(ui_element_t * el)
{
    widget_template_cfg_t * cfg = (widget_template_cfg_t *)el->ui_node->cfg;
    widget_template_ctx_t * ctx = (widget_template_ctx_t *)el->ctx;
}

static void draw(ui_element_t * el)
{
    widget_template_cfg_t * cfg = (widget_template_cfg_t *)el->ui_node->cfg;
    widget_template_ctx_t * ctx = (widget_template_ctx_t *)el->ctx;
}

static void select(ui_element_t * el)
{
    widget_template_cfg_t * cfg = (widget_template_cfg_t *)el->ui_node->cfg;
    widget_template_ctx_t * ctx = (widget_template_ctx_t *)el->ctx;
}

static unsigned process_event(ui_element_t * el, unsigned event)
{
    widget_template_cfg_t * cfg = (widget_template_cfg_t *)el->ui_node->cfg;
    widget_template_ctx_t * ctx = (widget_template_ctx_t *)el->ctx;
    return 0;
}

const widget_desc_t __widget_template = {
    .draw = draw,
    .calc = calc,
    .select = select,
    .process_event = process_event,
    .ctx_size = sizeof(widget_template_ctx_t)
};
