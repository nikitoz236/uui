#include "widget__test_text.h"
#include "draw_color.h"

static draw(ui_element_t * el)
{
    draw_frame(&el->f, 2, COLOR(0xAABBCC));
}

const widget_desc_t widget__test_text = {
    .ctx_size = WIDGET_CTX_SIZE(struct widget_ctx_struct__test_text),
    .draw = draw
};
