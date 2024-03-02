#include "time_moment.h"

#include "time.h"

static struct timespec systick_start;

void init_systick(void)
{
    clock_gettime(CLOCK_REALTIME, &systick_start);
}

void time_moment_save(time_moment_t * tm)
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    tm->ms = (t.tv_sec - systick_start.tv_sec) * 1000;
    tm->ms += (t.tv_nsec - systick_start.tv_nsec) / 1000000;
    tm->systick = (t.tv_nsec - systick_start.tv_nsec) % 1000000;
}

unsigned time_moment_interval_cycles(time_moment_t * start, time_moment_t * end)
{
    return 0;
}

unsigned time_manager_interval_us(time_moment_t * old, time_moment_t * new)
{
    return 0;
}
