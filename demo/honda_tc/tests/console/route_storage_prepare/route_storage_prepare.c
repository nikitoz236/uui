#include <stdio.h>

#include "routes.h"
#include "date_time.h"

#include "storage.h"

struct rdata {
    unsigned dist;
    unsigned fuel;
    unsigned time_h;
    unsigned time_m;
    unsigned time_s;
    date_t since_date;
    time_t since_time;
};

const unsigned odo_start = 245022000;
const struct rdata rdata[ROUTE_TYPE_NUM_SAVED] = {
    [ROUTE_TYPE_TOTAL] =        { .dist = 354101123, .fuel = 5000000, .time_h = 5678, .time_m = 34, .time_s = 30, .since_date = { .y = 2022, .m =  1, .d = 21 }, .since_time = { .h = 13, .m = 34, .s = 23 } },
    [ROUTE_TYPE_TRAVEL] =       { .dist = 3456432,   .fuel = 285123,  .time_h = 119,  .time_m = 12, .time_s = 34, .since_date = { .y = 2024, .m = 11, .d = 13 }, .since_time = { .h = 14, .m =  3, .s = 44 } },
    [ROUTE_TYPE_JOURNEY] =      { .dist = 9912345,   .fuel = 789456,  .time_h = 281,  .time_m = 37, .time_s = 19, .since_date = { .y = 2024, .m =  9, .d =  7 }, .since_time = { .h = 16, .m = 54, .s = 51 } },
    [ROUTE_TYPE_TMP] =          { .dist = 54321,     .fuel = 4001,    .time_h = 1,    .time_m = 28, .time_s = 54, .since_date = { .y = 2025, .m =  2, .d =  6 }, .since_time = { .h = 11, .m = 34, .s = 23 } },
    [ROUTE_TYPE_DAY] =          { .dist = 456123,    .fuel = 59876,   .time_h = 6,    .time_m = 21, .time_s = 45, .since_date = { .y = 2025, .m =  2, .d =  6 }, .since_time = { .h =  9, .m = 11, .s = 34 } },
    [ROUTE_TYPE_OIL_CHANGE] =   { .dist = 6789012,   .fuel = 543210,  .time_h = 251,  .time_m = 53, .time_s = 47, .since_date = { .y = 2024, .m =  4, .d = 22 }, .since_time = { .h = 21, .m = 33, .s = 17 } },
    [ROUTE_TYPE_ENGINE_MAINT] = { .dist = 97654321,  .fuel = 987654,  .time_h = 512,  .time_m = 41, .time_s = 27, .since_date = { .y = 2023, .m =  6, .d =  3 }, .since_time = { .h = 13, .m = 45, .s = 23 } },
    [ROUTE_TYPE_REFILL] =       { .dist = 123456,    .fuel = 11345,   .time_h = 1,    .time_m = 33, .time_s = 12, .since_date = { .y = 2025, .m =  2, .d =  4 }, .since_time = { .h = 14 ,.m = 45, .s = 16 } },
};


unsigned rdata_time_s(const struct rdata * rdata)
{
    return rdata->time_h * 3600 + rdata->time_m * 60 + rdata->time_s;
}

int main()
{
    printf("prepare routes and history storage file\r\n");

    storage_init();

    storage_write_file(800, &odo_start, sizeof(odo_start));

    unsigned total_time_s = rdata_time_s(&rdata[ROUTE_TYPE_TOTAL]);
    unsigned total_dist = rdata[ROUTE_TYPE_TOTAL].dist - odo_start;

    for (unsigned i = 0; i < ROUTE_TYPE_NUM_SAVED; i++) {
        unsigned since_s = days_to_s(days_from_date(&rdata[i].since_date)) + time_to_s(&rdata[i].since_time);

        unsigned rsaved[ROUTE_VAL_LOADABLE];

        if (i == ROUTE_TYPE_TOTAL) {
            rsaved[ROUTE_VALUE_DIST] = total_dist;
            rsaved[ROUTE_VALUE_FUEL] = rdata[i].fuel;
            rsaved[ROUTE_VALUE_TIME] = total_time_s;
        } else {
            rsaved[ROUTE_VALUE_DIST] = total_dist - rdata[i].dist;
            rsaved[ROUTE_VALUE_FUEL] = rdata[ROUTE_TYPE_TOTAL].fuel - rdata[i].fuel;
            rsaved[ROUTE_VALUE_TIME] = total_time_s - rdata_time_s(&rdata[i]);
        }

        rsaved[ROUTE_VALUE_SINCE_TIME] = since_s;

        printf("route %1d    dist %10d    fuel %10d    time %10d    since t %10d\r\n",
            i,
            rsaved[ROUTE_VALUE_DIST],
            rsaved[ROUTE_VALUE_FUEL],
            rsaved[ROUTE_VALUE_TIME],
            rsaved[ROUTE_VALUE_SINCE_TIME]
        );

        storage_write_file(801 + i, rsaved, sizeof(rsaved));
    }

    storage_print_info();

    return 0;
}
