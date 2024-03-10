#pragma once

extern unsigned uptime_ms;

typedef struct {
    unsigned timeout;
    unsigned start;
} mstimer_t;

static inline mstimer_t mstimer_with_timeout(unsigned timeout)
{
    return (mstimer_t){ .timeout = timeout };
}

static inline void mstimer_set_timeout(mstimer_t * t, unsigned timeout)
{
    t->timeout = timeout;
}

static inline void mstimer_reset(mstimer_t * t)
{
    t->start = uptime_ms;
}

static inline void mstimer_start_timeout(mstimer_t * t, unsigned timeout)
{
    t->timeout = timeout;
    t->start = uptime_ms;
}

static inline unsigned mstimer_get_time(mstimer_t * t)
{
    return uptime_ms - t->start;
}

static inline int __time_rel(unsigned t, unsigned ref)
{
    // > 0 when t before ref
    // < 0 when t after ref
    return (int)((int)ref - (int)t);
}

static inline unsigned mstimer_is_over(mstimer_t * t)
{
    if (__time_rel(uptime_ms,  t->start + t->timeout) < 0) {
        return 1;
    }
    return 0;
}

static inline unsigned mstimer_do_period(mstimer_t * t)
{
    if (mstimer_is_over(t)) {
        t->start += t->timeout;
        return 1;
    }
    return 0;
}
