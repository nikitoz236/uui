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

unsigned item_offset(lt_item_t * item)
{
    return (void *)item - (void *)mem_ptr;
}

lt_item_t * item_from_offset(unsigned offset)
{
    return (lt_item_t *)(mem_ptr + offset);
}

lt_item_t * lt_owner(lt_item_t * item)
{
    if (item->owner == 0) {
        return 0;
    }
    return item_from_offset(item->owner);
}

lt_item_t * lt_next(lt_item_t * item)
{
    if (item->next == 0) {
        return 0;
    }
    return item_from_offset(item->next);
}

static lt_item_t * lt_child(lt_item_t * item)
{
    if (item->child == 0) {
        return 0;
    }
    return item_from_offset(item->child);
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
    lt_item_t * item = item_from_offset(offset);

    if (owner) {
        owner_offset = item_offset(owner);
        if (owner->child) {
            prev_item = item_from_offset(owner->child);
        } else {
            // первый дочерний элемент
            owner->child = offset;
        }
    } else {
        if (mem_used) {
            // корневой элемент, но не первый в списке
            prev_item = item_from_offset(0);
        }
    }
    if (prev_item) {
        while (prev_item->next) {
            idx++;
            prev_item = item_from_offset(prev_item->next);
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