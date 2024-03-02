#include "time_moment.h"
#include "trip_integrate.h"

#define INJECTOR_PERFOMANCE_CC		235		// CC = cm3/min = ml/min

#define METRIC_ENUM_NUM(id, ...)        __INTEGRATE_VAR_ ## id

enum {
    TRIP_INTEGRATE_VAR_LIST(METRIC_ENUM_NUM),
    TRIP_INTEGRATE_VAR_NUM
};

time_moment_t last_time_ms;

unsigned last_speed = 0;
unsigned trip_dist = 0;         // max = (2 ** 32) / (3600 / 16) = 19088743 m
unsigned trip_fuel = 0;         // max = (2 ** 32) / (3600000 / 128) = 152709 ml

int integrated_metrics[TRIP_INTEGRATE_VAR_NUM] = {};

int trip_integrate_get_real(unsigned id)
{
    if (id < TRIP_INTEGRATE_VAR_NUM) {
        return integrated_metrics[id];
    }
    return 0;
}

unsigned trip_get_fuel_ml(void)
{
    // 1 ml
    return trip_fuel / (3600000 / 128);
}

unsigned trip_get_dist_m(void)
{
    // 1 m
    return trip_dist / (3600 / 16);
}

void trip_restart(void)
{
    trip_fuel = 0;
    trip_dist = 0;
}

static inline unsigned ms_from(time_moment_t * t)
{
    time_moment_t now;
    time_moment_save(&now);
    unsigned ms = time_moment_interval_ms(t, &now);
    *t = now;
    return ms;
}

/*
    по сути у нас есть трапеция - старое значение и новое значение, в также время между ними
    площадь равна площади прямоугольника высотой (новое значение + старое значение / 2) и шириной времени

    единицы и максимальные значения
        rpm                 4 на оборот, квант четверть оборота, максимум 9000 * 4 = 36000
        injection_time      250 на 1 мс, квант 4 мкс, максимум
        speed               1 на 1 км/ч, квант 1 км/ч, максимум 255


    например, при времени открытия 1 мс = (250 * 4) и оборотах 2400 = (9600 / 4) мы имеем время оборота (60 * 1000 / 2400) = 25 мс
    за оборот форсунка открывается на 2х цилиндрах, значит отношение будет 2 / 25 = 0.08,
    теперь если мы умножим 0.08 на 100 мс интервала между опросами то получим 8 мс открытого времени всеми форсунками за интервал
    при этом 250 * 9600 = 2400000

    например 4 мс при 4800 оборотах дадут нам 8 мс / 12.5 мс = 0.64 и 1000 * 4 * 4800 = 19200000 * 0.64 = 12288000
*/

void trip_integrate(unsigned rpm, unsigned speed, unsigned injection_time)
{
    unsigned ms = ms_from(&last_time_ms);
    integrated_metrics[__INTEGRATE_VAR_INTEGRATE_PERIOD] = ms;

    unsigned speed_per_interval = (speed + last_speed) / 2;
    last_speed = speed;

    // 1 km/h = 100000 cm/ 3600000 ms = (1/3600) m/ms = (1/36) cm/ms
    // dist_per_interval = m/ms * ms = (1/3600) m
    unsigned dist_per_interval = speed_per_interval * ms;
    trip_dist += dist_per_interval / 16;        // 3600 = 16 * 225

    // мгновенный расход мл в час = rpm(1/min) * injection_time(us) * perfomance(ml/min) * 2 / 1000000(us in s) = s * ml/min * min = ml / hour
    // факторизуем это значение 47 * 5 * 2 / (5 * 2 * (2 ^ 5) * 3125)
    #define INJECTORS_PER_REV   (2)
    #define FUEL_DIV_A          (1 << 5)
    #define FUEL_DIV_C          (5 * INJECTORS_PER_REV)
    #define FUEL_DIV_B          (US_IN_S / (FUEL_DIV_C * FUEL_DIV_A))
    #define FUEL_FACTOR         (INJECTOR_PERFOMANCE_CC * INJECTORS_PER_REV / FUEL_DIV_C)

    unsigned fuel_per_h = rpm * injection_time;
    fuel_per_h /= FUEL_DIV_A;
    fuel_per_h *= FUEL_FACTOR;
    fuel_per_h /= FUEL_DIV_B;

    unsigned fuel_per_dist = fuel_per_h * 100;
    fuel_per_dist /= speed_per_interval;

    unsigned last_fuel_per_h = integrated_metrics[__INTEGRATE_VAR_CONS_MOMENT_TIME];

    // fuel_per_interval = ml/hour * ms = ml / 3600 * 1000 = (1/3600000) ml
    unsigned fuel_per_interval = (fuel_per_h + last_fuel_per_h) / 2;
    fuel_per_interval *= ms;

    trip_fuel += fuel_per_interval / 128;       // 3600000 = 128 * 28125

    integrated_metrics[__INTEGRATE_VAR_CONS_MOMENT_TIME] = fuel_per_h;
    integrated_metrics[__INTEGRATE_VAR_CONS_MOMENT_DIST] = fuel_per_dist;
}
