#include "time.h"

static struct timespec rtc_start;
static unsigned rtc_time_s = 0;

void init_rtc(void)
{
    clock_gettime(CLOCK_REALTIME, &rtc_start);
}

unsigned rtc_get_time_s(void)
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    unsigned uptime_s = t.tv_sec - rtc_start.tv_sec;
    return rtc_time_s + uptime_s;
}

void rtc_set_time_s(unsigned time_s)
{
    rtc_time_s = time_s;
    init_rtc();
}
