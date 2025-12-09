#include "ltree.h"
#include "round_up.h"

static void * mem_ptr;
unsigned mem_used;
unsigned mem_size;

void lt_init(void * mem, unsigned size)
{
    mem_ptr = mem;
    mem_size = size;
    mem_used = 0;
}

unsigned lt_used(void)
{
    return mem_used;
}

unsigned item_size_in_mem(lt_item_t * item)
{
    return round_up_deg2(sizeof(lt_item_t) + item->desc->ctx_size, sizeof(uint32_t));
}

lt_item_t * lt_next_in_mem(lt_item_t * item)
{
    unsigned offset = lt_item_offset(item);
    offset += item_size_in_mem(item);
    if (offset >= mem_used) {
        return 0;
    }
    return lt_item_from_offset(offset);
}

unsigned lt_item_offset(lt_item_t * item)
{
    return (void *)item - (void *)mem_ptr;
}

lt_item_t * lt_item_from_offset(unsigned offset)
{
    return (lt_item_t *)(mem_ptr + offset);
}

lt_item_t * lt_owner(lt_item_t * item)
{
    if (item->owner == 0) {
        return 0;
    }
    return lt_item_from_offset(item->owner);
}

lt_item_t * lt_next(lt_item_t * item)
{
    if (item->next == 0) {
        return 0;
    }
    return lt_item_from_offset(item->next);
}

lt_item_t * lt_child(lt_item_t * item)
{
    if (item->child == 0) {
        return 0;
    }
    return lt_item_from_offset(item->child);
}

lt_item_t * lt_child_idx(lt_item_t * item, unsigned idx)
{
    lt_item_t * child = lt_child(item);
    for (unsigned i = 0; i < idx; i++) {
        child = lt_next(child);
    }
    return child;
}

lt_item_t * lt_add(lt_item_t * owner, const lt_desc_t * desc)
{
    unsigned owner_offset = 0;
    unsigned idx = 0;
    lt_item_t * prev_item = 0;

    unsigned offset = mem_used;
    lt_item_t * item = lt_item_from_offset(offset);

    if (owner) {
        owner_offset = lt_item_offset(owner);
        if (owner->child) {
            prev_item = lt_item_from_offset(owner->child);
        } else {
            // первый дочерний элемент
            owner->child = offset;
        }
    } else {
        if (mem_used) {
            // корневой элемент, но не первый в списке
            prev_item = lt_item_from_offset(0);
        }
    }
    if (prev_item) {
        while (prev_item->next) {
            idx++;
            prev_item = lt_item_from_offset(prev_item->next);
        }
        prev_item->next = offset;
    }

    item->desc = desc;

    item->owner = owner_offset;
    item->next = 0;
    item->child = 0;
    item->idx = idx;
    item->flags = 0;

    mem_used += item_size_in_mem(item);

    return item;
}



/*

тесты

удаление всего
добавление нескольких корневых элементов

удаление элемента в середине
удаление элемента в конце

добавление дочерних элементов


*/


uint16_t * link_to_item(lt_item_t * item)
{
    lt_item_t * owner = lt_owner(item);
    lt_item_t * pred = lt_child(owner);

    if (pred == item) {
        return &owner->child;
    }

    while (pred->next != lt_item_offset(item)) {
        pred = lt_next(pred);
    }

    return &pred->next;
}

void move_item(lt_item_t * new, lt_item_t * old)
{
    unsigned offset_new = lt_item_offset(new);

    // обновление ссылки на элемент
    *link_to_item(old) = offset_new;

    // перемещение элемента словами u32
    uint32_t * d = (uint32_t *) new;
    uint32_t * s = (uint32_t *) old;
    unsigned size = item_size_in_mem(old);
    while (size) {
        *d++ = *s++;
        size -= sizeof(uint32_t);
    }

    // обновление ссылок у дочерних элементов
    lt_item_t * child = lt_child(new);
    while (child) {
        child->owner = offset_new;
        child = lt_next(child);
    }
}

static void collect_deleted(void)
{
    lt_item_t * it = lt_item_from_offset(0);
    lt_item_t * del = 0;
    while (it) {
        if (it->owner == -1) {
            if (del == 0) {
                del = it;
            }
        } else {
            if (del) {
                move_item(del, it);
            }
        }
    }
}

void lt_delete_childs(lt_item_t * item)
{
    lt_item_t * child = lt_child(item);
    while (child) {
        child->owner = -1;
        child = lt_next(child);
    }
}

void lt_delete(lt_item_t * item)
{
    /*
        естественно элемент должен удаляться с дочерними

        желательно опрделить максимальный непрерывный участок для удаления
        дальше нужно умное перемещение в котором будут пересчитаны указатели


        как будто нужно помечать обьекты для удаления, а потом идти по массиву и перемещать неудаленные обьекты влево. обновляя ссылки на них
        значит обьекты можно двигать по одному, проход слева гарантирует что ссылкибудут обновляться лишь один раз. это у родителей и предыдущих, а child будут

    */
    *link_to_item(item) = item->next;
    item->owner = -1;

    lt_item_t * child = lt_child(item);
    while (child) {
        child->owner = -1;
        child = lt_next(child);
    }

}
