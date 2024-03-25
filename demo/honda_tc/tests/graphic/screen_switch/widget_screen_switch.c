#include "widget_screen_switch.h"

static void draw(ui_element_t * el)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;
    unsigned current_screen = *cfg->selector_ptr;
    ui_node_desc_t * node = &cfg->screens_list[current_screen];
    ui_element_t * item = ui_tree_add(el, node);
    item->f = el->f;
    ui_tree_element_draw(item);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;
    uint8_t * current_screen = cfg->selector_ptr;
    if (event == 1) {
        (*current_screen)++;
        if (*current_screen == cfg->screens_num) {
            *current_screen = 0;
        }
        draw(el);
        return 1;
    }
    return 0;
}

widget_desc_t __widget_screen_switch = {
    .draw = draw,
    .process_event = process
};
