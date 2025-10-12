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
    enum {
        TASK_TYPE_SINGLE,
        TASK_TYPE_IDX,
    } type;
} task_desc_t;

typedef struct {
    ll_item_t ll_item;
    const task_desc_t * desc;
} task_t;

#define TASK_INIT_IDX(func, num) { .desc = &(const task_desc_t) { .func_idx = func, .timers = TL_CREATE(num), .count = num, .type = TASK_TYPE_IDX } }
#define TASK_INIT_SINGLE(func) { .desc = &(const task_desc_t) { .func_noarg = func, .timers = TL_CREATE(1), .count = 1, .type = TASK_TYPE_SINGLE } }

void task_run_idx(task_t * task, unsigned idx, unsigned time_ms);
void task_stop_idx(task_t * task, unsigned idx);
static inline unsigned task_is_active_idx(task_t * task, unsigned idx) { return tl_is_active(task->desc->timers, idx); };
static inline unsigned task_remain_ms_idx(task_t * task, unsigned idx) { return tl_remain_ms(task->desc->timers, idx); };

static inline void task_run_single(task_t * task, unsigned time_ms) { task_run_idx(task, 0, time_ms); };
static inline void task_stop_single(task_t * task) { task_stop_idx(task, 0); };
static inline unsigned task_is_active_single(task_t * task) { return task_is_active_idx(task, 0); };
static inline unsigned task_remain_ms_single(task_t * task) { return task_remain_ms_idx(task, 0); };

void task_process(void);
