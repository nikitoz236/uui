#include "ui_tree.h"
#include "debug_print.h"

static void * ui_tree_ptr = 0;
static unsigned ui_tree_size = 0;
static unsigned ui_tree_top = 0;

static ui_element_t * ui_tree_element(uint16_t element_offset)
{
    return (ui_element_t *)((void *)ui_tree_ptr + element_offset);
}

static uint16_t element_offset(ui_element_t * el)
{
    return (void *)el - (void *)ui_tree_ptr;
}

static ui_element_t * add_node(const ui_node_desc_t * ui_node, unsigned owner_offset, unsigned idx)
{
    ui_element_t * el = ui_tree_element(ui_tree_top);
    ui_tree_top += sizeof(ui_element_t) + ui_node->widget->ctx_size;
    el->ui_node = ui_node;
    el->idx = idx;
    el->owner = owner_offset;
    el->child = 0;
    el->next = 0;
    el->active = 0;
    return el;
}

void ui_tree_init(void * ptr, unsigned size, const ui_node_desc_t * ui_node, const xy_t * display_size)
{
    ui_tree_ptr = ptr;
    ui_tree_size = size;
    ui_element_t * el = add_node(ui_node, 0, 0);
    el->f.s = *display_size;
    el->f.p = (xy_t){0, 0};

}

ui_element_t * ui_tree_owner(ui_element_t * element)
{
    return ui_tree_element(element->owner);
}

ui_element_t * ui_tree_child(ui_element_t * element)
{
    if (element->child == 0) {
        return 0;
    }
    return ui_tree_element(element->child);
}

ui_element_t * ui_tree_next(ui_element_t * element)
{
    if (element->next == 0) {
        return 0;
    }
    return ui_tree_element(element->next);
}

void * ui_tree_ctx(ui_element_t * element)
{
    return (void *)element->ctx;
}

static ui_element_t * search_last_child(ui_element_t * owner)
{
    ui_element_t * last = ui_tree_child(owner);
    while (last->next) {
        debug_print("    search last child %d\n", last->next);
        last = ui_tree_element(last->next);
    }
    return last;
}

ui_element_t * ui_tree_add(ui_element_t * owner, const ui_node_desc_t * ui_node)
{
    ui_element_t * el;
    debug_print("ui tree add owner %d, tree top %d\n", element_offset(owner), ui_tree_top);
    if (owner->child == 0) {
        el = add_node(ui_node, element_offset(owner), 0);
        owner->child = element_offset(el);
        debug_print("  first child. offset %d, tree top %d\n", owner->child, ui_tree_top);
    } else {
        ui_element_t * last = search_last_child(owner);
        el = add_node(ui_node, element_offset(owner), last->idx + 1);
        last->next = element_offset(el);
        debug_print("  next child. offset %d, tree top %d\n", owner->next, ui_tree_top);
    }
    return el;
}

static void move_tree(unsigned offset, unsigned size)
{
    unsigned source = offset + size;
    uint8_t * stack = (uint8_t *)ui_tree_ptr;
    while(source < ui_tree_top) {
        stack[offset] = stack[source];
        offset++;
        source++;
    }
    ui_tree_top -= size;
}

void ui_tree_delete_childs(ui_element_t * element)
{
    while (1) {
        ui_element_t * last = ui_tree_element(element->child);
        ui_element_t * previous = 0;
        while (last->next) {
            previous = last;
            last = ui_tree_element(last->next);
        }


        if (last->child) {
            ui_tree_delete_childs(last);
            /*
                очень не оптимальная история. надо бежать по
                указатели на следующие элементы в следующих элементах поломаются после удаления дочерних контекстов
                так что надо вычислять размер непрерывно удаляемой области
                либо переносить элементы осмысленно переписывая заголовки родителя
            */
        } else {
            move_tree(element_offset(last), last->ui_node->widget->ctx_size + sizeof(ui_element_t));
        }

        debug_print("        delete child %d, top %d\n", element_offset(last), ui_tree_top);

        if (previous == 0) {
            break;
        } else {
            previous->next = 0;
        }
    }
    element->child = 0;

}

static ui_element_t * ui_tree_next_linear(ui_element_t * element)
{
    unsigned offset = element_offset(element);
    offset += element->ui_node->widget->ctx_size + sizeof(ui_element_t);
    if (offset >= ui_tree_top) {
        return 0;
    }
    return ui_tree_element(offset);
}

void ui_tree_draw(void)
{
    ui_element_t * element = ui_tree_element(0);
    element->ui_node->widget->draw(element);
}

void ui_tree_update(void);

void ui_tree_process_event(unsigned event)
{
    ui_element_t * element = ui_tree_element(0);
    element->ui_node->widget->process_event(element, event);
}

void ui_tree_debug_print_linear(void)
{
    debug_print("ui tree linear:\n");
    ui_element_t * element = ui_tree_element(0);
    while (element) {
        debug_print("    element %d offset %4d, owner %4d, child %4d, next %4d, ctx size %d\n", element->idx, element_offset(element), element->owner, element->child, element->next, element->ui_node->widget->ctx_size);
        element = ui_tree_next_linear(element);
    }
}

static void ui_tree_debug_print_tree_element(ui_element_t * element, unsigned level)
{
    for (unsigned i = 0; i < level; i++) {
        debug_print("    ");
    }
    debug_print("element %d offset %4d, owner %4d, child %4d, next %4d, ctx size %d\n", element->idx, element_offset(element), element->owner, element->child, element->next, element->ui_node->widget->ctx_size);
    if (element->child) {
        ui_element_t * child = ui_tree_child(element);
        while (child) {
            ui_tree_debug_print_tree_element(child, level + 1);
            child = ui_tree_next(child);
        }
    }
}

void ui_tree_debug_print_tree(void)
{
    debug_print("ui tree tree:\n");
    ui_element_t * element = ui_tree_element(0);
    ui_tree_debug_print_tree_element(element, 1);
}
