#pragma once
#include <stdint.h>

/*
    отложенный вызов функции через time_ms мс

    зависимости:
        timers_32 - таймер на каждый слот таска
        ll        - список активных тасков
        eq        - очередь обработчиков, выполняемых в background
        uptime    - get_uptime_ms() - источник времени, кроссплатформенный

    использование:
        SINGLE - один обработчик void func(void)
            static task_t t = TASK_INIT_SINGLE(func);
            task_run_single(&t, ms);

        IDX - один обработчик void func(unsigned idx) на N слотов
            static task_t t = TASK_INIT_IDX(func, N);
            task_run_idx(&t, idx, ms);

        повторный run для того же idx перезапускает таймер.
        task_stop_idx(...) отменяет до срабатывания.

    в главном цикле дёргать:
        task_process();   - проверяет таймеры, истёкшие кладёт в eq
        eq_process();     - вызывает один обработчик из очереди

    размещение: task_t в RAM (внутри ll_item.next меняется),
    desc и timers - флеш и RAM соответственно через макрос TASK_INIT_*
*/

#include "timers_32.h"
#include "uptime.h"
#include "ll.h"


typedef struct {
    union {
        void (*func_noarg)(void);
        void (*func_idx)(unsigned idx);
    };
    timer_32_t * timers;
    uint8_t count;
    enum {
        TASK_TYPE_SINGLE,
        TASK_TYPE_IDX,
    } type : 1;
} task_desc_t;

typedef struct {
    ll_item_t ll_item;
    const task_desc_t * desc;
} task_t;

#define TASK_INIT_IDX(func, num) { .desc = &(const task_desc_t) { .func_idx = func, .timers = (timer_32_t [num]){}, .count = num, .type = TASK_TYPE_IDX } }
#define TASK_INIT_SINGLE(func) { .desc = &(const task_desc_t) { .func_noarg = func, .timers = (timer_32_t [1]){}, .count = 1, .type = TASK_TYPE_SINGLE } }

void task_run_idx(task_t * task, unsigned idx, unsigned time_ms);
void task_stop_idx(task_t * task, unsigned idx);
static inline unsigned task_is_active_idx(task_t * task, unsigned idx) { return t32_is_active(&task->desc->timers[idx], get_uptime_ms()); };
static inline unsigned task_remain_ms_idx(task_t * task, unsigned idx) { return t32_remain(&task->desc->timers[idx], get_uptime_ms()); };

static inline void task_run_single(task_t * task, unsigned time_ms) { task_run_idx(task, 0, time_ms); };
static inline void task_stop_single(task_t * task) { task_stop_idx(task, 0); };
static inline unsigned task_is_active_single(task_t * task) { return task_is_active_idx(task, 0); };
static inline unsigned task_remain_ms_single(task_t * task) { return task_remain_ms_idx(task, 0); };

void task_process(void);
