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
        // if (ce->ui_node->widget->draw) {
        //     ce->ui_node->widget->draw(ce);
        // }
    }
}

ui_node_desc_t ui_node_16 = { .widget = &(widget_desc_t){ .ctx_size = 5}, .cfg = 0 };
ui_node_desc_t ui_node_8 = { .widget = &(widget_desc_t){ .ctx_size = 4, .draw = ui_node_draw }, .cfg = &(ui_node_cfg_t){ .node = &ui_node_16, .num = 2 }};
ui_node_desc_t ui_node_4 = { .widget = &(widget_desc_t){ .ctx_size = 3, .draw = ui_node_draw }, .cfg = &(ui_node_cfg_t){ .node = &ui_node_8, .num = 1 }};
ui_node_desc_t ui_node_2 = { .widget = &(widget_desc_t){ .ctx_size = 2, .draw = ui_node_draw }, .cfg = &(ui_node_cfg_t){ .node = &ui_node_4, .num = 3 }};

/*


*/

ui_element_t * ui_tree_next_linear(ui_element_t * element);

int main() {
    printf("UI Tree Element Delete Test\n");


    uint8_t ui_stack[1024];
    ui_tree_init(ui_stack, sizeof(ui_stack), &ui_node_2, &(xy_t){333, 555});
    ui_tree_draw();

    ui_element_t * el = (ui_element_t *)ui_stack;
    unsigned n = 10;
    while (el) {
        el->ctx[0] = n;
        n++;
        el = ui_tree_next_linear(el);
    }

    ui_tree_debug_print_linear();
    ui_tree_debug_print_tree();

    el = (ui_element_t *)ui_stack;
    el = ui_tree_child(el);
    el = ui_tree_child(el);

    printf("\n delete childs of 14 element\n");
    ui_tree_delete_childs(el);

    ui_tree_debug_print_linear();
    ui_tree_debug_print_tree();

    printf("add childs to 14 element\n");

    ui_tree_add(el, &ui_node_2)->ctx[0] = n++;
    ui_tree_add(el, &ui_node_2)->ctx[0] = n++;

    ui_tree_debug_print_linear();
    ui_tree_debug_print_tree();

    printf("delete childs of 12 element\n");

    el = (ui_element_t *)ui_stack;
    el = ui_tree_child(el);
    el = ui_tree_next(el);

    ui_tree_delete_childs(el);

    ui_tree_debug_print_linear();
    ui_tree_debug_print_tree();

    printf("add childs to 12 element\n");

    ui_tree_add(el, &ui_node_8)->ctx[0] = n++;
    ui_tree_add(el, &ui_node_8)->ctx[0] = n++;

    ui_tree_debug_print_linear();
    ui_tree_debug_print_tree();

    /*

        element 10: offset: 0, 0x5662f088 idx  0, owner    0, child   24, next    0, ctx size 2
        element 11: offset: 24, 0x5662f064 idx  0, owner    0, child   96, next   48, ctx size 3
            element 14: offset: 96, 0x5662f040 idx  0, owner   24, child  224, next    0, ctx size 4
                element 23: offset: 224, 0x5662f088 idx  0, owner   96, child    0, next  248, ctx size 2
                element 24: offset: 248, 0x5662f088 idx  1, owner   96, child    0, next    0, ctx size 2
        element 12: offset: 48, 0x5662f064 idx  1, owner    0, child  272, next   72, ctx size 3
            element 25: offset: 272, 0x5662f040 idx  0, owner   48, child    0, next  296, ctx size 4
            element 26: offset: 296, 0x5662f040 idx  1, owner   48, child    0, next    0, ctx size 4
        element 13: offset: 72, 0x5662f064 idx  2, owner    0, child  144, next    0, ctx size 3
            element 20: offset: 144, 0x5662f040 idx  0, owner   72, child  168, next    0, ctx size 4
                element 21: offset: 168, 0x5662f01c idx  0, owner  144, child    0, next  196, ctx size 5
                element 22: offset: 196, 0x5662f01c idx  1, owner  144, child    0, next    0, ctx size 5

    */

    return 0;
}
