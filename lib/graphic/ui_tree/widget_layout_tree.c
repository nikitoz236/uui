#include "widget_layout_tree.h"


static void draw(ui_element_t * el)
{
    __widget_layout_tree_cfg_t * cfg = (__widget_layout_tree_cfg_t *)el->ui_node->cfg;
    unsigned selected_layout = *cfg->selected_layout;

    unsigned cidx = 0;
    for (unsigned y = 0; y < cfg->size_list[selected_layout].h; y++) {
        for (unsigned x = 0; x < cfg->size_list[selected_layout].w; x++) {
            ui_element_t * ce = ui_tree_add(el, &cfg->node_list[cidx]);
            printf("layout mode %d add cidx: %d x %d, y %d, ", selected_layout, cidx, x, y);
            form_grid(&el->f, &ce->f, &cfg->borders, &cfg->gaps, &cfg->size_list[selected_layout], x, y);
            printf("form p (%d %d) s (%d %d)\n", ce->f.p.x, ce->f.p.y, ce->f.s.w, ce->f.s.h);
            ce->ui_node->widget->draw(ce);
            cidx++;
        }
    }
}

static unsigned process_event(ui_element_t * el, unsigned event)
{
    __widget_layout_tree_cfg_t * cfg = (__widget_layout_tree_cfg_t *)el->ui_node->cfg;
    if (event == 1) {
        printf("event change selected layout %d, ", *cfg->selected_layout);

        // (*cfg->selected_layout)++;
        // if (*cfg->selected_layout >= cfg->size_list_len) {
        //     *cfg->selected_layout = 0;
        // }

        if (*cfg->selected_layout == 0) {
            *cfg->selected_layout = cfg->size_list_len;
        }
        (*cfg->selected_layout)--;

        printf("new val %d\n", *cfg->selected_layout);
        ui_tree_delete_childs(el);
        draw(el);
        return 1;
    }
    return 0;
}

const widget_desc_t __widget_layout_tree = {
    .draw = draw,
    .process_event = process_event,
};
