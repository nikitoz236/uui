#pragma once
#include <stdint.h>

/*
    struct s {};
    list_t * list_of_s = LIST(((struct s []){ {}, {}, {}}));
    void process_s(struct s * s) {};
    list_process_all(list_of_s, (void(*)(void *))process_s)



*/

typedef struct {
    uint16_t count;
    uint16_t step;
    // макрос LIST когда будет создавать поле items со сложной структурой бует выравнивать его по размеру машинного слова
    uint8_t data[] __attribute__ ((aligned (sizeof(int *))));
} list_t;

#define LIST(array) (list_t *)&(const struct { list_t header; typeof(array[0]) items[sizeof(array)/sizeof(array[0])]; } ){ .header = { .count = sizeof(array)/sizeof(array[0]), .step = sizeof(array[0]) }, .items = array }

static inline void __list_process_item(list_t * l, unsigned idx, void(*process)(void *))
{
    if (idx < l->count) {
        process(&l->data[l->step * idx]);
    }
}

static inline void __list_process_all(list_t * l, void(*process)(void *))
{
    for (unsigned i = 0; i < l->count; i++) {
        process(&l->data[l->step * i]);
    }
}

#define list_process_item(l, f) __list_process_item(l, (void(*)(void *))f)
#define list_process_all(l, f) __list_process_all(l, (void(*)(void *))f)

// static inline void list_process_item(list_t * l, unsigned offset, unsigned idx, void(*process)(void *))
// {
//     if (idx < l->count) {
//         void * item = l->list + offset + (idx * l->step);
//         process(item);
//     }
// }

// static inline void list_process_field(list_t * l, unsigned offset, void(*process)(void *))
// {
//     for (unsigned i = 0; i < l->count; i++) {
//         void * item = l->list + offset + (i * l->step);
//         process(item);
//     }
// }
