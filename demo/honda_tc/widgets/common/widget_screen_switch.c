#include "widget_screen_switch.h"
#include "tc_events.h"

/*
    вообще смотри. есть 2 парадигмы

    виджет задумывался как некоторый конфигурируемый виджет, которых может быть много. и каждый чтото свое из списка показывает. Тоесть все виджеты до последнего думают должны ли они данное событие обрабатывать

    либо мы делаем чтобы каждый виджет имел режим ( контекст ) и понимал выбран он или нет, нужно ли ему передавать события глубже или нет ?
*/

static void draw(ui_element_t * el)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;
    unsigned current_screen = *cfg->selector_ptr;
    ui_node_desc_t * node = &cfg->screens_list[current_screen];
    ui_element_t * item = ui_tree_add(el, node, current_screen);
    item->f = el->f;
    ui_tree_element_draw(item);
}

static void draw_new_child(ui_element_t * el)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;
    ui_tree_delete_childs(el);
    draw(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;

    ui_element_t * item = ui_tree_child(el);

    uint8_t * current_screen = cfg->selector_ptr;
    if (event == EVENT_BTN_DOWN) {
        (*current_screen)++;
        if (*current_screen == cfg->screens_num) {
            *current_screen = 0;
        }
        draw_new_child(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        if (*current_screen == 0) {
            *current_screen = cfg->screens_num;
        }
        (*current_screen)--;

        draw_new_child(el);
        return 1;
    }
    if (event == EVENT_BTN_OK) {
        ui_tree_element_select(item, 1);
        return 1;
    }
    if (event == EVENT_BTN_LEFT) {
        ui_tree_element_select(item, 0);
        return 1;
    }
    return 0;
}

widget_desc_t __widget_screen_switch = {
    .draw = draw,
    .process_event = process,

};
