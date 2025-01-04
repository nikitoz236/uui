#include "ui_tree.h"
#include "debug_print.h"
#include "misc.h"

// #define debug__create    debug_print
#define debug__create

static void * ui_tree_ptr = 0;
static unsigned ui_tree_size = 0;
static unsigned ui_tree_top = 0;

unsigned ui_tree_len(void)
{
    return ui_tree_top;
}

static inline ui_element_t * ui_tree_element(uint16_t element_offset)
{
    return (ui_element_t *)((void *)ui_tree_ptr + element_offset);
}

static inline uint16_t element_offset(ui_element_t * el)
{
    return (void *)el - (void *)ui_tree_ptr;
}

static inline unsigned element_size(ui_element_t * el)
{
    return round_up_deg2(sizeof(ui_element_t) + el->ui_node->widget->ctx_size, sizeof(uint32_t));
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
    el->active = 1;
    ui_tree_element_calc(el);
    if (display_size) {
        el->f.s = *display_size;
    }
    el->f.p = (xy_t){0, 0};
    ui_tree_element_extend(el);
}

ui_element_t * ui_tree_owner(ui_element_t * element)
{
    return ui_tree_element(element->owner);
}

// first child
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

ui_element_t * ui_tree_child_idx(ui_element_t * el, unsigned idx)
{
    ui_element_t * item = ui_tree_child(el);
    for (unsigned i = 0; i < idx; i++) {
        item = ui_tree_next(item);
    }
    return item;
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

ui_element_t * ui_tree_add(ui_element_t * owner, const ui_node_desc_t * ui_node, unsigned idx)
{
    ui_element_t * el;
    debug__create("ui tree add owner %d, tree top %d\n", element_offset(owner), ui_tree_top);
    if (owner->child == 0) {
        // первый дочерний элемент
        el = add_node(ui_node, element_offset(owner), 0);
        owner->child = element_offset(el);
    } else {
        ui_element_t * last = search_last_child(owner);
        el = add_node(ui_node, element_offset(owner), last->idx + 1);
        last->next = element_offset(el);
    }
    el->idx = idx;
    return el;
}

// следующий элемент в памяти
static ui_element_t * ui_tree_next_linear(ui_element_t * element)
{
    unsigned offset = element_offset(element);
    offset += element_size(element);
    if (offset >= ui_tree_top) {
        return 0;
    }
    return ui_tree_element(offset);
}



    /*

unsigned some_recursive(unsigned * start, unsigned * previous)
{
    // удаляем с какогото элемента цепочки вместе с детями

    unsigned this_previous = 0;
    unsigned cont_start = *start;
    unsigned len = calc_elements_continuous_len(&cont_start, &this_previous);
    if (this_previous != 0) {
        // ну точно придется еще раз пройтись но уже обозначив конец
        ui_tree_element(this_previous)->next = 0;
        // some_recursive(start);
    }

    // а зесь мы уже имеем непрерывную цепочку, надо проверить теперь ее чаилдов

    unsigned child_position = cont_start + len;
    unsigned offset = cont_start;
    while (1) {
        ui_element_t * el = ui_tree_element(offset);
        if (el->child) {
            if (el->child == child_position) {
                unsigned child_previous = 0;
                unsigned child_offset = el->child;
                // кажется тут надо прогнать рекурсию

                unsigned child_len = some_recursive(&child_offset, &child_previous);
                if (child_previous != 0) {
                    ui_tree_element(child_previous)->next = 0;
                    // тут есть разрыв между последним элементом и началом чайлдов
                } else {
                    // тут все хорошо
                }
            } else {
                // тут есть разрыв между последним элементом и началом чайлдов
            }
        }
        offset += el->next;
    }

    // должна быть куча вызовов рекурсии выше и здесь состояние когда нам вернули какой - то конечный интервал. возможно весь
        move_tree(cont_start, len);

    /*
    итак мы имеем элемент, его поле чайлд ссылается на цепочку которую будем удалять

    выделяем интервал начиная с начала цепочки, и проверяем лежат ли все нексты друг за другом ?
    если нет то обнуляем старт интервала

    для каждого элемента в цепочке если есть чайлд то надо для него сделать тоже самое, и на выходе мы будем знать его интервал.
    и вот тут нам надо надеяться что он соприкасается с нашим интервалом

    мы удаляем дерева с конца и со дна

    возможны варианты:
        интервал начинается с середины цепочки
            нужно обнулить ссылку на первый элемент интервала в предыдущем элементе
            в этом сценарии чайлды тоже могут оказаться в интервале
        интервал начинается от начала цепочки - зануляем чайлд владельца


    */

    // вот тут вайл в котором мы ищем последоватлеьные элементы

    // если элементы не последовательны то запускаем себя начиная с последнего сайза тоесть какбы вниз по стеку ближе к концу

    // и вот если оказалось что до конца вся цепочка в текущем вызове то удаляем все элементы начиная с текущего оффсета


// }

















static void move_tree(unsigned offset, unsigned size)
{
    unsigned from = offset + size;
    unsigned * src = (unsigned *)(ui_tree_ptr + from);
    unsigned * dst = (unsigned *)(ui_tree_ptr + offset);

    // debug_print("move tree from %d to %d, size %d\n", from, offset, size);

    // фактически перенос если после удаляемой области есть что-то еще
    while(from < ui_tree_top) {
        *dst = *src;
        src++;
        dst++;
        from += sizeof(unsigned);
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

static void ui_tree_delete_child_chain(ui_element_t * child);

// функция ищет такой офсет start в цепочке, чтобы все элементы начиная с него до конца цепочки шли в памяти подряд
// если будет разрыв то в previous запишется офсет предыдущего элемента, а в start офсет первого элемента непрерывной цепочки
static unsigned calc_elements_continuous_len(unsigned * start, unsigned * previous)
{
    unsigned len = 0;
    unsigned offset = *start;

    // один раз пробегаем по всей цепочке
    while (1) {
        ui_element_t * el = ui_tree_element(offset);
        unsigned el_len = element_size(el);

        // важно чтобы у элементов цепочки не было чайлдов
        if (el->child) {
            ui_tree_delete_child_chain(ui_tree_child(el));
            el->child = 0;
        }

        if (el->next == 0) {
            // для последнего элемента возвращаем длину цепочки и его собственную длину
            len += el_len;
            break;
        }

        // думаем не рвется ли цепочка
        unsigned next_linear = offset + el_len;
        if (el->next == next_linear) {
            len += el_len;
        } else {
            // следующий элемент не находится после текущего - цепочка порвалась
            // передвигаем старт цепочки
            *previous = element_offset(el);
            *start = el->next;
            len = 0;
        }

        offset = el->next;
    }
    return len;
}

static void ui_tree_delete_child_chain(ui_element_t * child)
{
    while (1) {
        unsigned previous = 0;
        unsigned delete_child_offset = element_offset(child);
        unsigned len = calc_elements_continuous_len(&delete_child_offset, &previous);
        move_tree(delete_child_offset, len);
        if (previous == 0) {
            break;
        } else {
            ui_element_t * previous_el = ui_tree_element(previous);
            previous_el->next = 0;
        }
    }
}

void ui_tree_delete_childs(ui_element_t * element)
{
    /*
        сам элемиент оставляем в дереве, а для всех чайлдов вызываем ui_tree_delete_child_chain
        которая с помощью calc_elements_continuous_len понимает какой интервал в памяти непрерывный можно удалить
        после удаляет его с помощью move_tree
        если calc_elements_continuous_len встречает элемент с дочерними элементами то для каждого вызывает ui_tree_delete_child_chain
    */

    if (element->child) {
        ui_tree_delete_child_chain(ui_tree_child(element));
        element->child = 0;
    }
}

void ui_tree_element_update(ui_element_t * element)
{
    if (element->ui_node->widget->update) {
        element->ui_node->widget->update(element);
    }
}

void ui_tree_element_calc(ui_element_t * element)
{
    if (element->ui_node->widget->calc) {
        element->ui_node->widget->calc(element);
    }
}

void ui_tree_element_extend(ui_element_t * el)
{
    if (el->ui_node->widget->extend) {
        el->ui_node->widget->extend(el);
    }
}

void ui_tree_element_draw(ui_element_t * element)
{
    if (element->ui_node->widget->draw) {
        element->ui_node->widget->draw(element);
    } else {
        ui_element_t * child = ui_tree_child(element);
        while (child) {
            ui_tree_element_draw(child);
            child = ui_tree_next(child);
        }
    }
}

void ui_tree_element_select(ui_element_t * element, unsigned select)
{
    element->active = select;
    if (element->ui_node->widget->select) {
        element->ui_node->widget->select(element);
    }
}

void ui_tree_draw(void)
{
    ui_element_t * element = ui_tree_element(0);
    ui_tree_element_draw(element);
}

void ui_tree_update(void)
{
    ui_element_t * element = ui_tree_element(0);
    ui_tree_element_update(element);
}

static unsigned ui_tree_element_process(ui_element_t * element, unsigned event)
{
    unsigned result = 0;
    ui_element_t * child = ui_tree_child(element);

    while (child) {
        if (ui_tree_element_process(child, event)) {
            result = 1;
        }

        if (child->ui_node->widget->update) {
            child->ui_node->widget->update(child);
        }

        child = ui_tree_next(child);
    }

    if (event) {
        if (result == 0) {
            if (element->active) {
                if (element->ui_node->widget->process_event) {
                    result = element->ui_node->widget->process_event(element, event);
                }
            }
        }
    }

    return result;
}

void ui_tree_process(unsigned event)
{
    ui_element_t * element = ui_tree_element(0);
    ui_tree_element_process(element, event);
}




// debug prints

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

void print_el_offset(ui_element_t * element)
{
    debug_print("element %d: ", element_offset(element));
}

void ui_tree_debug_print_tree(void)
{
    debug_print("ui tree tree:\n");
    ui_element_t * element = ui_tree_element(0);
    ui_tree_debug_print_tree_element(element, 1);
}
