#include "ui_tree.h"
#include "debug_print.h"
#include "misc.h"

// #define debug__create    debug_print
#define debug__create

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

static inline unsigned element_size(ui_element_t * el)
{
    return round_up_deg2(sizeof(ui_element_t) + el->ui_node->widget->ctx_size, 4);
}

static ui_element_t * add_node(const ui_node_desc_t * ui_node, unsigned owner_offset, unsigned idx)
{
    ui_element_t * el = ui_tree_element(ui_tree_top);
    el->ui_node = ui_node;
    el->idx = idx;
    el->owner = owner_offset;
    el->child = 0;
    el->next = 0;
    el->active = 0;

    // after save ui_node pointer
    ui_tree_top += element_size(el);
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
        debug__create("    search last child %d\n", last->next);
        last = ui_tree_element(last->next);
    }
    return last;
}

ui_element_t * ui_tree_add(ui_element_t * owner, const ui_node_desc_t * ui_node)
{
    ui_element_t * el;
    debug__create("ui tree add owner %d, tree top %d\n", element_offset(owner), ui_tree_top);
    if (owner->child == 0) {
        el = add_node(ui_node, element_offset(owner), 0);
        owner->child = element_offset(el);
        debug__create("  first child. offset %d, tree top %d\n", owner->child, ui_tree_top);
    } else {
        ui_element_t * last = search_last_child(owner);
        el = add_node(ui_node, element_offset(owner), last->idx + 1);
        last->next = element_offset(el);
        debug__create("  next child. offset %d, tree top %d\n", owner->next, ui_tree_top);
    }
    return el;
}

ui_element_t * ui_tree_next_linear(ui_element_t * element)
{
    unsigned offset = element_offset(element);
    offset += element_size(element);
    if (offset >= ui_tree_top) {
        return 0;
    }
    return ui_tree_element(offset);
}

static void move_tree(unsigned offset, unsigned size)
{
    unsigned source = offset + size;
    unsigned * src = (unsigned *)(ui_tree_ptr + source);
    unsigned * dst = (unsigned *)(ui_tree_ptr + offset);

    debug_print("move tree from %d to %d, size %d\n", source, offset, size);

    while(source < ui_tree_top) {
        *dst = *src;
        src++;
        dst++;
        source += sizeof(unsigned);
    }

    ui_tree_top -= size;

    unsigned last_offset = offset + size;
    ui_element_t * element = ui_tree_element(0);
    while (element) {
        // нужно актуализировать все ссылки в дереве если они ссылались в область которую переместили
        if (element->next >= last_offset) {
            element->next -= size;
        }
        if (element->child >= last_offset) {
            element->child -= size;
        }
        if (element->owner >= last_offset) {
            element->owner -= size;
        }
        element = ui_tree_next_linear(element);
    }
}


// static unsigned calc_row_len(unsigned offset)
// {
//     unsigned len = 0;
//     unsigned start = offset;
//     unsigned last = 0;


//     // сначала мы находим последний непрерывный кусок
//     while (1) {
//         ui_element_t * el = ui_tree_element(offset);
//         unsigned el_len = element_size(el);
//         if (el->next) {
//             unsigned next_linear = offset + el_len;
//             if (el->next == next_linear) {
//                 len += el_len;
//             } else {
//                 start = el->next;
//                 len = 0;
//             }
//         } else {
//             break;
//         }
//         offset = el->next;
//     }

//     потом начиная от старта бежим по чайлдам и считаем их размер






//     if (el->next == next_linear) {
//         len += el_len;
//     } else {
//         len = 0;
//         start = el->next;
//     }


//     ui_element_t * element = ui_tree_element(offset);

//     while (element) {
//         len += element_size(element);
//         element = ui_tree_next_linear(element);
//     }
//     return len;
// }

void ui_tree_delete_childs(ui_element_t * element)
{
    debug_print("delete childs of %d\n", element_offset(element));
    while (1) {
        ui_element_t * last = ui_tree_element(element->child);
        unsigned start = element_offset(last);
        unsigned len = 0;
        ui_element_t * previous = 0;

/*
итак мы имеем элемент, его поле чайлд ссылается на цепочку которую будем удалять

выделяем интервал начиная с начала цепочки, и проверяем лежат ли все нексты друг за другом ?
если нет то обнуляем старт интервала

для каждого элемента в цепочке если есть чайлд то надо для него сделать тоже самое, и на выходе мы будем знать его интервал. и вот тут нам надо надеяться что он соприкасается с нашим интервалом 

мы удаляем дерева с конца и со дна

возможны варианты:
    интервал начинается с середины цепочки
        нужно обнулить ссылку на первый элемент интервала в предыдущем элементе
        в этом сценарии чайлды тоже могут оказаться в интервале
    интервал начинается от начала цепочки - зануляем чайлд владельца


*/

        

        //  ищем последний элемент последовательности, также надо запомнить последнего кто на него ссылался чтобы ему поставить 0
        while (last->next) {
            unsigned this_offset = element_offset(last);
            unsigned this_len = element_size(last);
            unsigned next_linear = this_offset + this_len;
            if (next_linear == last->next) {
                len += this_len;
            } else {
                start = this_offset;
                previous = last;
            }
            debug_print("- search interval for delete, start %d, len %d, prev %d\n", start, len, element_offset(previous));
            last = ui_tree_element(last->next);
        }
        // last последний элемент
        len += element_size(last);


        if (last->child) {
            ui_tree_delete_childs(last);
            /*
                очень не оптимальная история. надо бежать по
                указатели на следующие элементы в следующих элементах поломаются после удаления дочерних контекстов
                так что надо вычислять размер непрерывно удаляемой области
                либо переносить элементы осмысленно переписывая заголовки родителя
            */
        } else {
            move_tree(start, len);
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


void ui_tree_element_draw(ui_element_t * element)
{
    if (element->ui_node->widget->draw) {
        element->ui_node->widget->draw(element);
    }
    ui_element_t * child = ui_tree_child(element);
    while (child) {
        ui_tree_element_draw(child);
        child = ui_tree_next(child);
    }
}

void ui_tree_draw(void)
{
    ui_element_t * element = ui_tree_element(0);
    ui_tree_element_draw(element);
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
        // debug_print_hex("--  ", element, sizeof(ui_element_t) + element->ui_node->widget->ctx_size);
        debug_print_color(GREEN, NONE, "    element %d: ", element->ctx[0]);
        debug_print("offset: %d, %p idx %2d, owner %4d, child %4d, next %4d, ctx size %d\n",  element_offset(element), element->ui_node, element->idx, element->owner, element->child, element->next, element->ui_node->widget->ctx_size);
        element = ui_tree_next_linear(element);
    }
}

static void ui_tree_debug_print_tree_element(ui_element_t * element, unsigned level)
{
    for (unsigned i = 0; i < level; i++) {
        debug_print("    ");
    }
    debug_print_color(RED, NONE, "element %d: ", element->ctx[0]);
    debug_print("offset: %d, %p idx %2d, owner %4d, child %4d, next %4d, ctx size %d\n",  element_offset(element), element->ui_node, element->idx, element->owner, element->child, element->next, element->ui_node->widget->ctx_size);
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
