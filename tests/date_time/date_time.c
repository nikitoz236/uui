#include <stdint.h>
#include <stdio.h>
#include "date_time.h"

void test(int * flag, char * name, int val, int expect)
{
    printf("  TEST: %s \t\t result: %5d \t\texpect: %5d \t\t", name, val, expect);
    if (val != expect) {
        *flag = 1;
        printf("FAIL\r\n");
    } else {
        printf("OK\r\n");
    }
}

void test_date(int * flag, char * name, date_t * d, date_t * expect)
{
    printf("  TEST: %s \t\t result: %02d.%02d.%02d \texpect: %02d.%02d.%02d\t", name, d->y, d->m + 1, d->d, expect->y, expect->m + 1, expect->d);
    if ((d->y != expect->y) || (d->m != expect->m) || (d->d != expect->d)) {
        *flag = 1;
        printf("FAIL\r\n");
    } else {
        printf("OK\r\n");
    }
}

void test_time(int * flag, char * name, time_t * d, time_t * expect)
{
    printf("  TEST: %s \t\t result: %02d:%02d:%02d \texpect: %02d:%02d:%02d\t", name, d->h, d->m, d->s, expect->h, expect->m, expect->s);
    if ((d->h != expect->h) || (d->m != expect->m) || (d->s != expect->s)) {
        *flag = 1;
        printf("FAIL\r\n");
    } else {
        printf("OK\r\n");
    }
}


int main()
{
    int flag = 0;
    date_t tmp_date;
    time_t tmp_time;
    int d;
    int s;

    printf("Date Time test\r\n");

    test(&flag, "leap 2022            ", is_year_leap(2022), 0);
    test(&flag, "leap 2028            ", is_year_leap(2028), 1);
    test(&flag, "leap 1990            ", is_year_leap(1990), 0);
    test(&flag, "leap 1920            ", is_year_leap(1920), 1);
    test(&flag, "leap 1900            ", is_year_leap(1900), 0);

    test(&flag, "days in month JUL 1960", days_in_month(MONTH_JUL, 1960), 31);
    test(&flag, "days in month FEB 1960", days_in_month(MONTH_FEB, 1960), 29);
    test(&flag, "days in month FEB 1977", days_in_month(MONTH_FEB, 1977), 28);

    test(&flag, "day_of_week 22 03 27  ", day_of_week(&(date_t){.y = 2022, .m = MONTH_MAR, .d = 27 }), WEEK_DAY_SUN);
    test(&flag, "day_of_week 90 04 21  ", day_of_week(&(date_t){.y = 1990, .m = MONTH_APR, .d = 21 }), WEEK_DAY_SAT);

    test(&flag, "days of date 00 01 01 ", days_from_date(&(date_t){ .y = 2000, .m = MONTH_JAN, .d = 1 }), 0);
    test(&flag, "days of date 00 01 02 ", days_from_date(&(date_t){ .y = 2000, .m = MONTH_JAN, .d = 2 }), 1);
    test(&flag, "days of date 00 02 01 ", days_from_date(&(date_t){ .y = 2000, .m = MONTH_FEB, .d = 1 }), 31);
    test(&flag, "days of date 22 03 27 ", days_from_date(&(date_t){ .y = 2022, .m = MONTH_MAR, .d = 27 }), 8121);

    d = days_from_date(&(date_t){ .y = 2022, .m = MONTH_MAR, .d = 27 });
    date_from_days(&tmp_date, d + 7);
    test_date(&flag, "days of date 22 03 27 ", &tmp_date, &(date_t){ .y = 2022, .m = MONTH_APR, .d = 3 });

    time_from_s(&tmp_time, 1);
    test_time(&flag, "time from sec 1      ", &tmp_time, &(time_t){ .h = 0, .m = 0, .s = 1 });

    time_from_s(&tmp_time, 20052);
    test_time(&flag, "time from sec 1      ", &tmp_time, &(time_t){ .h = 5, .m = 34, .s = 12 });



    if (flag) {
        printf("FAIL\r\n");
    }

    return flag;
}
