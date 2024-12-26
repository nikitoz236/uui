#include "date_time.h"
#include "str_utils.h"

#include "str_utils.h"
#include "str_val.h"

#define WEEK_DAY_NAME(name)         #name
#define MONTH_NAME(name)            #name

static const char * week_day_names[] = {
    WEEK_DAY_LIST(WEEK_DAY_NAME)
};

#define SEC_IN_MIN                  60
#define MIN_IN_HOUR                 60
#define HOURS_IN_DAY                24

#define SEC_IN_HOUR                 (SEC_IN_MIN * MIN_IN_HOUR)
#define SEC_IN_DAY                  (SEC_IN_HOUR * HOURS_IN_DAY)
#define YEAR_START                  2000

unsigned is_year_leap(unsigned y)
{
    if (y & (4 - 1)) {                // не делится на 4
        return 0;
    }
    if (((y % 100) == 0) && ((y % 400) != 0)) {
        return 0;
    }
    return 1;
}

unsigned days_in_month(month_t month, unsigned year)
{
    //                                   J   F   M   A   M   J   J   A   S   O   N   D
    static const uint8_t month_len[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    uint8_t d = month_len[month];
    if (month == MONTH_FEB) {
        if (is_year_leap(year)) {
            d += 1;
        }
    }
    return d;
}

week_day_t day_of_week(date_t * d)
{
    uint16_t y = d->y;
    uint8_t m;
    if (d->m < MONTH_MAR) {
        y -= 1;
        m = d->m + 1 + 10;
    } else {
        m = d->m + 1 - 2;
    }

    unsigned wd = d->d - 1;
    wd += (m * 31) / 12;
    wd += y;
    wd += y / 4;
    wd -= y / 100;
    wd += y / 400;
    return wd % 7;
}

unsigned days_from_s(unsigned s)
{
    return s / SEC_IN_DAY;
}

unsigned days_to_s(unsigned days)
{
    return days * SEC_IN_DAY;
}

unsigned days_from_date(date_t * d)
{
    uint8_t year = (d->y - YEAR_START);
    unsigned days = year * 365;

    if (year) {
        days += ((year - 1) / 4) + 1;               // ??? високосные ???
    }

    for (unsigned i = MONTH_JAN; i < d->m; i++) {
        days += days_in_month(i, d->y);
    }
    days += (d->d - 1);                             // day = 1 ... 31
    return days;
}

void date_from_days(date_t * d, unsigned days)
{
    d->y = YEAR_START;
    d->m = MONTH_JAN;
    d->d = 1;

    unsigned days_in_year;
    unsigned month_days;
    while (days) {
        if (is_year_leap(d->y)) {
            days_in_year = 366;
        } else {
            days_in_year = 365;
        }

        if (days >= days_in_year) {
            days -= days_in_year;
            d->y++;
        } else {
            month_days = days_in_month(d->m, d->y);

            if (days >= month_days) {
                days -= month_days;
                d->m++;
            } else {
                d->d += days;
                days = 0;
            }
        }
    }
}

void date_from_s(date_t * d, unsigned s)
{
    date_from_days(d, days_from_s(s));
}

void time_from_s(time_t * t, unsigned s)
{
    t->s = s % SEC_IN_MIN;
    s /= SEC_IN_MIN;
    t->m = s % MIN_IN_HOUR;
    s /= MIN_IN_HOUR;
    t->h = s % HOURS_IN_DAY;
}

unsigned time_to_s(time_t * t)
{
    return t->s + t->m * SEC_IN_MIN + t->h * SEC_IN_HOUR;
}

void date_yy_to_str(date_t * d, char * c);
void date_yyyy_to_str(date_t * d, char * c);


void time_hh_mm_to_str(time_t * t, char * str)
{
    dec_to_str_right_aligned(t->h, &str[0], 2, 1);
    dec_to_str_right_aligned(t->m, &str[3], 2, 1);
    str[2] = ':';
    str[5] = 0;
}

void time_hh_mm_ss_to_str(time_t * t, char * str)
{
    time_hh_mm_to_str(t, str);
    dec_to_str_right_aligned(t->s, &str[6], 2, 1);
    str[5] = ':';
    str[8] = 0;
}

void date_dd_mname_yyyy_to_str(date_t * d, char * str)
{
    static const char * month_names[] = {
        MONTH_LIST(MONTH_NAME)
    };
    dec_to_str_right_aligned(d->d, &str[0], 2, 1);
    str_cp(&str[3], month_names[d->m], 3);
    dec_to_str_right_aligned(d->y, &str[7], 4, 1);
    str[2] = ' ';
    str[6] = ' ';
    str[11] = 0;
}
