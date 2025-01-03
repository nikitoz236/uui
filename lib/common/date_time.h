#pragma once
#include <stdint.h>

#define WEEK_DAY_LIST(m) \
    m(MON), \
    m(TUE), \
    m(WED), \
    m(THU), \
    m(FRI), \
    m(SAT), \
    m(SUN)

#define MONTH_LIST(m) \
    m(JAN), \
    m(FEB), \
    m(MAR), \
    m(APR), \
    m(MAY), \
    m(JUN), \
    m(JUL), \
    m(AUG), \
    m(SEP), \
    m(OCT), \
    m(NOV), \
    m(DEC)

#define WEEK_DAY_ENUM(name)         WEEK_DAY_ ## name
#define MONTH_ENUM(name)            MONTH_ ## name

typedef enum {
    WEEK_DAY_LIST(WEEK_DAY_ENUM)
} week_day_t;                       // 0 - MON ... 6 - SUN

typedef enum {
    MONTH_LIST(MONTH_ENUM)
} month_t;                          // 0 - JAN ... 11 - DEC

typedef struct {
    uint8_t d;                      // 1 ... 31
    month_t m;                      // 0 ... 11
    uint16_t y;                     // 0 ... 2022 ... 65535
} date_t;

typedef struct {
    uint8_t h;                      // 0 ... 23
    uint8_t m;                      // 0 ... 59
    uint8_t s;                      // 0 ... 59
} time_t;

unsigned is_year_leap(unsigned y);
unsigned days_in_month(month_t m, unsigned y);
week_day_t day_of_week(date_t * d);

unsigned days_from_s(unsigned s);
unsigned days_to_s(unsigned days);
unsigned days_from_date(date_t * d);

void date_from_days(date_t * d, unsigned days);
void date_from_s(date_t * d, unsigned s);
void time_from_s(time_t * t, unsigned s);
unsigned time_to_s(time_t * t);

void date_yy_to_str(date_t * d, char * c);
void date_yyyy_to_str(date_t * d, char * c);
void time_hh_mm_ss_to_str(time_t * t, char * str);
void time_hh_mm_to_str(time_t * t, char * str);

void date_dd_mname_yyyy_to_str(date_t * d, char * c);
