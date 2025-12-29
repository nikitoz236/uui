#include <stdint.h>
#include "soc/systimer_struct.h"

#include "time_rel.h"


static void systimer_sync_val(unsigned unit)
{
    SYSTIMER.unit_op[unit].timer_unit_update = 1;
    while (SYSTIMER.unit_op[unit].timer_unit_value_valid == 0) {};
}

uint64_t systimer_get_time(unsigned unit)
{
    systimer_sync_val(unit);
    uint64_t val = SYSTIMER.unit_val[unit].hi.val;
    val <<= 32;
    val += SYSTIMER.unit_val[unit].lo.val;
    return val;
}

uint32_t systimer_us(unsigned unit)
{
    systimer_sync_val(unit);
    uint32_t us = SYSTIMER.unit_val[unit].lo.val >> 4;
    us += SYSTIMER.unit_val[unit].hi.val << 28;
    // dp("us read = "); dpd(us, 10); dn();
    return us;
}

uint32_t systimer_ms(unsigned unit)
{
    systimer_sync_val(unit);
    /*
        итак время это (((2 ** 32) * hi) + lo) / (16 * 1000) = (lo / 16000) + (hi * (2 ** (32 - 4)) / 1000)
    */
    // uint64_t ticks = systimer_get_time(unit) / 16000;
    // return ticks;

    uint32_t ms = SYSTIMER.unit_val[unit].lo.val / 16000;
    uint32_t hi = SYSTIMER.unit_val[unit].hi.val << 16;
    hi /= 1000;
    hi <<= 12;
    ms += hi;
    // dp("ms read = "); dpd(ms, 10); dn();
    return ms;
}

void delay_ms(unsigned time)
{
    volatile unsigned start = systimer_ms(0);
    while (1) {
        volatile unsigned now = systimer_ms(0);
        if (__time_rel(start, now) >= (int)time) {
            break;
        }
    }
}

void delay_us(unsigned time)
{
    unsigned start = systimer_us(0);
    while (1) {
        unsigned now = systimer_us(0);
        if (__time_rel(start, now) >= (int)time) {
            break;
        }
    }
}
