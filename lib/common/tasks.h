#pragma once
#include <stdint.h>

#include "tl.h"
#include "ll.h"


typedef struct {
    union {
        void (*func_noarg)(void);
        void (*func_idx)(unsigned idx);
    };
    tl_t * timers;
    uint8_t count;
} task_desc_t;

typedef struct {
    ll_item_t ll_item;
    const task_desc_t * desc;
} task_t;

#define TASK_CREATE(func, num)

void task_run_idx(task_t * task, unsigned idx, unsigned time_ms);

static inline unsigned task_is_active_idx(task_t * task, unsigned idx)
{
    return tl_is_active(task->desc->timers, idx);
}

void task_process(void);