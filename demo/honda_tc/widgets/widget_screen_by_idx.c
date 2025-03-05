#include "widget_screen_by_idx.h"

static void draw(ui_element_t * el)
{
    widget_screen_by_idx_cfg_t * cfg = (widget_screen_by_idx_cfg_t *)el->ui_node->cfg;
    if (el->idx < cfg->screens_num) {
        ui_element_t * item = ui_tree_add(el, &cfg->screens_list[el->idx], 0);
        printf("draw screen by idx %d, form %d %d %d %d\r\n", el->idx, item->f.p.x, item->f.p.y, item->f.s.w, item->f.s.h);
        ui_tree_element_draw(item);
    }
}

const widget_desc_t widget_screen_by_idx = {
    .draw = draw,
};
