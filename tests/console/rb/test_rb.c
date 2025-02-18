#include <stdio.h>

#include "rb.h"

// typedef struct __attribute__((packed)) {
typedef struct {
    rb_desc_t rb;
    uint8_t dma_ch;
} dma_rb_t;

struct {
    rb_t ri;
    uint8_t data[5];
} rb_ctx;

const rb_desc_t rb_desc = {
    .rb = &rb_ctx.ri,
    .size = 5
};


static inline void put(uint8_t val)
{
    printf("< before head %d, tail %d, is_empty %d, is_full %d, available %d  ", rb_desc.rb->head, rb_desc.rb->tail, rb_is_empty(&rb_desc), rb_is_full(&rb_desc), rb_available(&rb_desc));
    printf("put: %3d", val);
    rb_put(&rb_desc, val);
    printf("  after head %d, tail %d, is_empty %d, is_full %d, available %d\n", rb_desc.rb->head, rb_desc.rb->tail, rb_is_empty(&rb_desc), rb_is_full(&rb_desc), rb_available(&rb_desc));
}

static inline void get(void)
{
    printf("> before head %d, tail %d, is_empty %d, is_full %d, available %d  ", rb_desc.rb->head, rb_desc.rb->tail, rb_is_empty(&rb_desc), rb_is_full(&rb_desc), rb_available(&rb_desc));
    printf("get: %3d", rb_get(&rb_desc));
    printf("  after head %d, tail %d, is_empty %d, is_full %d, available %d\n", rb_desc.rb->head, rb_desc.rb->tail, rb_is_empty(&rb_desc), rb_is_full(&rb_desc), rb_available(&rb_desc));
}

int main()
{
    printf("rb test\n");

    printf("rb_desc_t size: %d\n", sizeof(rb_desc_t));
    printf("dma_rb_t size: %d\n", sizeof(dma_rb_t));

    printf("rb empty available %d\n", rb_available(&rb_desc));

    uint8_t val = 0;

    for (unsigned i = 0; i < 10; i++) {
        printf("\n-------------\n\n");
        while (!rb_is_full(&rb_desc)) {
            put(val++);
        }
        printf("\n");
        while (!rb_is_empty(&rb_desc)) {
            get();
        }
    }

    // printf("rb after get 12 el available %d\n", rb_available(&rb_desc));

    // for (unsigned i = 0; i < 10; i++) {
    //     printf("rb get %d, rb is empty %d, is full %d, available %d, head %d, tail %d\n", rb_get(&rb_desc), rb_is_empty(&rb_desc), rb_is_full(&rb_desc), rb_available(&rb_desc), rb_desc.rb->head, rb_desc.rb->tail);
    //     rb_put(&rb_desc, i);
    // }



    return 0;
}
