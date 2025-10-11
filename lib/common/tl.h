/*
    timers list

    работа с группой таймеров


    tl_run - запустить таймер
    tl_stop - остановить таймер

    функции проверки, сами не останавливают таймер, вызвать tl_stop для деактивации

    tl_remain_ms - проверить сколько осталось времени, сам не останавливает таймер
    tl_is_active - проверить активен ли таймер
    tl_is_over - проверить вышло ли время

    ну тоесть мы можем проверять из разных мест is_active, is_over и т.д.
    однако по выходу времени is_over будет возвращать вызвали задачу то для остановки таймера нужно вызвать 


*/

#pragma once

#include "mstimer.h"
#include "bit_fields.h"
#include "flex_wrap.h"

struct tl {
    unsigned active;
    unsigned time_ms[];
};

#define TL_CREATE(num)  FLEX_WRAP_ELEMENTS(struct tl, unsigned, num)

static inline void tl_run(struct tl * t, unsigned idx, unsigned time_ms)
{
    if (idx >= (sizeof(unsigned) * 8)) {
        return;
    }
    t->time_ms[idx] = uptime_ms + time_ms;
    bf_set(&t->active, idx, 1, 1);
}

static inline void tl_extend(struct tl * t, unsigned idx, unsigned time_ms)
{
    if (idx >= (sizeof(unsigned) * 8)) {
        return;
    }
    t->time_ms[idx] += time_ms;
    bf_set(&t->active, idx, 1, 1);
}

static inline void tl_stop(struct tl * t, unsigned idx)
{
    if (idx >= (sizeof(unsigned) * 8)) {
        return;
    }
    bf_set(&t->active, idx, 1, 0);
}

static inline unsigned tl_remain_ms(struct tl * t, unsigned idx)
{
    if (idx >= (sizeof(unsigned) * 8)) {
        return 0;
    }
    if (bf_get(&t->active, idx, 1)) {
        int rel = __time_rel(uptime_ms, t->time_ms[idx]);
        if (rel > 0) {
            return rel;
        } else {
            // время вышло
            // bf_set(&t->active, idx, 1, 0);
        }
    }
    return 0;
}

static inline unsigned tl_is_active(struct tl * t, unsigned idx)
{
    if (idx >= (sizeof(unsigned) * 8)) {
        return 0;
    }
    if (bf_get(&t->active, idx, 1)) {
        if (__time_rel(uptime_ms, t->time_ms[idx]) > 0) {
            return 1;
        }
        // время вышло
        // bf_set(&t->active, idx, 1, 0);
    }
    return 0;
}

static inline unsigned tl_is_over(struct tl * t, unsigned idx)
{
    if (idx >= (sizeof(unsigned) * 8)) {
        return 0;
    }
    if (bf_get(&t->active, idx, 1)) {
        if (__time_rel(uptime_ms, t->time_ms[idx]) <= 0) {
            // время вышло
            // bf_set(&t->active, idx, 1, 0);
            return 1;
        }
    }
    return 0;
}
