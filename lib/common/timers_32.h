#pragma once
#include <stdint.h>

typedef union {
    struct {
        uint32_t active : 1;
        uint32_t time : 31;
    };
    uint32_t val;
} timer_32_t;

#include "time_rel.h"
static inline int __t32_rel(timer_32_t * timer, uint32_t current_time)
{
    return __time_rel(current_time << 1, timer->val) >> 1;
}

static inline void t32_run(timer_32_t * timer, uint32_t current_time, unsigned time)
{
    timer->time = current_time + time;
    timer->active = 1;
}

static inline void t32_extend(timer_32_t * timer, unsigned time)
{
    timer->time += time;
    timer->active = 1;
}

static inline void t32_stop(timer_32_t * timer)
{
    timer->active = 0;
}

static inline unsigned t32_is_active(timer_32_t * timer, uint32_t current_time)
{
    if (timer->active) {
        if (__t32_rel(timer, current_time) > 0) {
            return 1;
        }
    }
    return 0;
}

static inline unsigned t32_is_over(timer_32_t * timer, uint32_t current_time)
{
    if (timer->active) {
        if (__t32_rel(timer, current_time) <= 0) {
            return 1;
        }
    }
    return 0;
}

static inline unsigned t32_remain(timer_32_t * timer, uint32_t current_time)
{
    if (timer->active) {
        int rel = __t32_rel(timer, current_time);
        if (rel > 0) {
            return rel;
        }
    }
    return 0;
}
