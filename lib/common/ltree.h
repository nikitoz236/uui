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

    каждый элемент имеет указатель на константное описание

    реализация использует смещения вместо указателей для экономии памяти
*/

// контекстная часть ноды в буффере
struct lt_item {
    /*
        Указатель на указатель намеренно — ltree реализует абстрактное дерево
        и не знает о структуре дескриптора конкретного применения. Снаружи

        (в ui) ctx_size указывает на структуру описателя виджета
            struct ui_node_desc {
                const struct widget_desc * widget;
                const void * cfg;
            };

            struct widget_desc {
                uint16_t ctx_size;
                ...
            };

        ltree читает первое поле widget_desc_t через **ctx_size, не зная об остальных полях.

        юнион позволяет обращаться к одному указателю двумя способами:
        - desc     — для передачи наружу (lt_item_desc), снаружи кастуется к нужной структуре
        - ctx_size — для чтения размера контекста внутри ltree через **ctx_size
     */
    union {
        const void * desc;
        const uint16_t **  ctx_size;
    };
    uint16_t owner;
    uint16_t next;
    uint16_t child;

    uint8_t ctx[] __attribute__ ((aligned(sizeof(uint32_t))));
};

typedef struct lt_item lt_item_t;

void lt_init(void * mem, unsigned size);

unsigned lt_used(void);

unsigned lt_item_offset(lt_item_t * item);
lt_item_t * lt_item_from_offset(unsigned offset);

lt_item_t * lt_next_in_mem(lt_item_t * item);

lt_item_t * lt_add(lt_item_t * owner, const void * desc);

lt_item_t * lt_owner(lt_item_t * item);
lt_item_t * lt_next(lt_item_t * item);
lt_item_t * lt_child(lt_item_t * item);
lt_item_t * lt_child_idx(lt_item_t * item, unsigned idx);

void lt_delete(lt_item_t * item);
void lt_delete_childs(lt_item_t * item);

// возвращает указатель на ui_node_desc (описатель ноды) — аналог el->ui_node в ui_tree
// снаружи кастуется к нужной структуре дескриптора
static inline const void * lt_item_desc(lt_item_t * item)
{
    return item->desc;
}

static inline void * lt_item_ctx(lt_item_t * item)
{
    return item->ctx;
}
