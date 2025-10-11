#pragma once
#include <stdint.h>

typedef struct {
    void * list;
    uint16_t count;
    uint16_t step;
} list_t;

static inline void list_process_item(list_t * l, unsigned offset, unsigned idx, void(*process)(void *))
{
    if (idx < l->count) {
        void * item = l->list + offset + (idx * l->step);
        process(item);
    }
}

static inline void list_process_field(list_t * l, unsigned offset, void(*process)(void *))
{
    for (unsigned i = 0; i < l->count; i++) {
        void * item = l->list + offset + (i * l->step);
        process(item);
    }
}
