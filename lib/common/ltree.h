#pragma once

#include <stdint.h>

/*
    это я пытаюсь выделить реализацию ui дерева, чтобы покрыть тестами, может научиться нормально удалять элементы

    модуль реализует в буффере односвязный список элементов
    каждый элемент может иметь дочерний список
    каждый элемент ссылается на родителя

    память алоцируется непрерывно слева, соблюдая вложенность для оптимизации удаления
        тоесть мы не можем удалить элемент из начала списка ? вообще можем.
        элемент удаляется с потомками

    каждый элемент имеет индекс
    каждый элемент имеет указатель на константное описание

    при выравнивании остается место под флаги

    реализация использует смещения вместо указателей для экономии памяти
*/

// const описатель ноды
struct lt_desc {
    uint16_t ctx_size;
    uint16_t desc_size;

    uint8_t desc[] __attribute__ ((aligned(sizeof(uint32_t))));
};

// контекстная часть ноды в буффере
struct lt_item {
    const struct lt_desc * desc;
    uint16_t owner;
    uint16_t next;
    uint16_t child;
    uint8_t idx;
    uint8_t flags;
    // как будто индексы и флаги это проблемы контекста, такчто можно их выпилить

    uint8_t ctx[] __attribute__ ((aligned(sizeof(uint32_t))));
};

typedef struct lt_desc lt_desc_t;
typedef struct lt_item lt_item_t;

void lt_init(void * mem, unsigned size);

unsigned lt_used(void);

unsigned lt_item_offset(lt_item_t * item);
lt_item_t * lt_item_from_offset(unsigned offset);

lt_item_t * lt_next_in_mem(lt_item_t * item);

lt_item_t * lt_add(lt_item_t * owner, const lt_desc_t * desc);

lt_item_t * lt_owner(lt_item_t * item);
lt_item_t * lt_next(lt_item_t * item);
lt_item_t * lt_child(lt_item_t * item);
lt_item_t * lt_child_idx(lt_item_t * item, unsigned idx);

void lt_delete(lt_item_t * item);
void lt_delete_childs(lt_item_t * item);

static inline const void * lt_item_desc(lt_item_t * item)
{
    return item->desc->desc;
}

static inline void * lt_item_ctx(lt_item_t * item)
{
    return item->ctx;
}
