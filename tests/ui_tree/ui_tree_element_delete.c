#include <stdio.h>
#include "ui_tree.h"

// что мы хотим проверить

// удаление элемента из дерева и перенос всего оставшегося дерева

typedef struct {
    ui_node_desc_t * node;
    unsigned num;
} ui_node_cfg_t;

void ui_node_draw(ui_element_t * el)
{
    ui_node_cfg_t * cfg = (ui_node_cfg_t *)el->ui_node->cfg;
    for (unsigned i = 0; i < cfg->num; i++) {
        ui_element_t * ce = ui_tree_add(el, cfg->node);
        ce->idx = i;
        if (ce->ui_node->widget->draw) {
            ce->ui_node->widget->draw(ce);
        }
    }
}

ui_node_desc_t ui_node_16 = { .widget = &(widget_desc_t){ .ctx_size = 16 }, .cfg = 0 };
ui_node_desc_t ui_node_8 = { .widget = &(widget_desc_t){ .ctx_size = 8, .draw = ui_node_draw }, .cfg = &(ui_node_cfg_t){ .node = &ui_node_16, .num = 4 }};
ui_node_desc_t ui_node_4 = { .widget = &(widget_desc_t){ .ctx_size = 4, .draw = ui_node_draw }, .cfg = &(ui_node_cfg_t){ .node = &ui_node_8, .num = 2 }};

int main() {
    printf("UI Tree Element Delete Test\n");


    uint8_t ui_stack[1024];
    ui_tree_init(ui_stack, sizeof(ui_stack), &ui_node_4, &(xy_t){333, 555});
    ui_tree_draw();

    ui_tree_debug_print_linear();
    ui_tree_debug_print_tree();

    return 0;
}
