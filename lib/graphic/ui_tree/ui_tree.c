#include "ui_tree.h"

static void * ui_tree_ptr = 0;
static unsigned ui_tree_size = 0;

static ui_element_t * ui_tree_element(uint16_t element_offset)
{
    return (ui_element_t *)((void *)ui_tree_ptr + element_offset);
}

void ui_tree_init(void * ptr, unsigned size, const ui_node_desc_t * ui_node, const xy_t * display_size)
{
    ui_tree_ptr = ptr;
    ui_tree_size = size;
    ui_element_t * el = ui_tree_element(0);
    el->f.s = *display_size;
    el->f.p = (xy_t){0, 0};
    el->ui_node = ui_node;
    el->owner = 0;
    el->next = 0;
    el->child = 0;
    el->idx = 0;
    el->active = 0;
}

static inline ui_element_t * ui_tree_owner(ui_element_t * element)
{
    return ui_tree_element(element->owner);
}

ui_element_t * ui_tree_child(ui_element_t * element)
{
    return ui_tree_element(element->child);
}

ui_element_t * ui_tree_next(ui_element_t * element)
{
    return ui_tree_element(element->next);
}

void * ui_tree_ctx(ui_element_t * element)
{
    return (void *)element->ctx;
}





    // создать элемент, прописать владельца, прописать следующего, прописать владельца следующего, прописать владельца ребенка, прописать следующего ребенка,



void ui_tree_delete(ui_element_t * element);

void ui_tree_draw(void)
{
    ui_element_t * element = ui_tree_element(0);
    element->ui_node->widget->draw(element);
}

void ui_tree_update(void);
void ui_tree_process_event(unsigned event);

