#include "widget__test_text.h"
#include "draw_color.h"

static void draw(ui_element_t * el)
{
    widget_cfg__test_text_t * cfg = (widget_cfg__test_text_t *)ui_node_desc(el)->cfg;
    draw_frame(&el->f, 2, cfg->frame_color);
}

const widget_desc_t widget__test_text = {
    .ctx_size = WIDGET_CTX_NONE,
    .draw = draw
};
