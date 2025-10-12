#include <stdint.h>
#include "rb.h"

struct eq_item {
    union {
        void (*func_noarg)(void);
        void (*func_arg)(unsigned);
    };
    uint8_t idx;
    uint16_t has_arg : 1;
    // enum {
    //     ARG_NONE,
    //     ARG_U8,
    //     ARG_U16,
    //     ARG_U32
    // } arg_type;
};

#define EQ_SIZE 16

const rb_desc_t eq = RB_DESC_INIT(struct eq_item, EQ_SIZE);

void eq_func_single(void (*func)(void))
{
    if (!rb_is_full(&eq)) {
        struct eq_item * item = (struct eq_item *)rb_element_put(&eq);
        item->func_noarg = func;
        item->has_arg = 0;
    }
}

void eq_func_idx(void (*func)(unsigned), unsigned idx)
{
    if (!rb_is_full(&eq)) {
        struct eq_item * item = (struct eq_item *)rb_element_put(&eq);
        item->func_arg = func;
        item->idx = idx;
        item->has_arg = 1;
    }
}

// всегда вызывается в background
unsigned eq_process(void)
{
    if (rb_is_empty(&eq)) {
        return 0;
    }
    struct eq_item * item = (struct eq_item *)rb_element_get(&eq);
    if (item->has_arg) {
        item->func_arg(item->idx);
    } else {
        item->func_noarg();
    }
    return 1;
}
