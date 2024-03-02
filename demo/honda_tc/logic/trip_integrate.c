#include "time_moment.h"
#include "trip_integrate.h"

#define INJECTOR_PERFOMANCE_CC		235		// CC = cm3/min = ml/min

#define METRIC_ENUM_NUM(id, ...)        __INTEGRATE_VAR_ ## id

enum {
    TRIP_INTEGRATE_VAR_LIST(METRIC_ENUM_NUM),
    TRIP_INTEGRATE_VAR_NUM
};

int integrated_metrics[TRIP_INTEGRATE_VAR_NUM] = {};

typedef struct {
    unsigned last_diff;
    time_moment_t time_ms;
    unsigned integrated;
} integrated_val;

integrated_val fuel = {};
integrated_val dist = {};

int trip_integrate_get_real(unsigned id)
{
    if (id < TRIP_INTEGRATE_VAR_NUM) {
        return integrated_metrics[id];
    }
    return 0;
}

unsigned trip_get_fuel(void)
{
    return fuel.integrated;
}

unsigned trip_get_dist(void)
{
    return dist.integrated;
}

void trip_restart(void)
{
    fuel.integrated = 0;
    dist.integrated = 0;
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
*/

void trip_integrate_speed(unsigned speed)
{
    unsigned ms = ms_from(&dist.time_ms);
    unsigned speed_per_interval = (speed + dist.last_diff) / 2;

    // в часу 3600000 мс, dist_per_interval имеет размерность (1 / 36) сантиметра
    // интегрировать будем в (1 / 9) сантиметра - переполнение 32 битной переменной возникнет через 4772.185 км
    // так будет быстрее работать функция trip_integrate_speed и мы сохраним максимальную точность с адекватным пробегом

    unsigned dist_per_interval = speed_per_interval * ms;
    dist.integrated += dist_per_interval / 4;
    dist.last_diff = speed;
}

// коэффициент на который нужно умножить (rpm * injection_time) чтобы получить расход топлива в мл/ч
#define FUEL_FACTOR         (INJECTOR_PERFOMANCE_CC * 2 / US_IN_S)
// факторизуем это значение 47 * 5 * 2 / (5 * 2 * (2 ^ 5) * 3125)
#define FUEL_FACTOR_DA      (2 ^ 5)
#define FUEL_FACTOR_DB      (US_IN_S / (10 * FUEL_FACTOR_DA))
#define FUEL_FACTOR_M       (FUEL_FACTOR * FUEL_FACTOR_DA * FUEL_FACTOR_DB)

void trip_integrate_injectors(unsigned injection_time, unsigned rpm)
{
    unsigned ms = ms_from(&fuel.time_ms);

    // мгновенный расход мл в час = rpm(1/min) * injection_time(us) * perfomance(ml/min) * 2 / 1000000 = s * ml/min * min = ml / hour
    unsigned fuel_per_h = rpm * injection_time;
    fuel_per_h /= FUEL_FACTOR_DA;
    fuel_per_h *= FUEL_FACTOR_M;
    fuel_per_h /= FUEL_FACTOR_DB;

    unsigned cons_dist = fuel_per_h * 100;
    unsigned speed = dist.last_diff;
    cons_dist /= speed;

    unsigned fuel_per_interval = (fuel_per_h + fuel.last_diff) / 2;
    fuel_per_interval *= ms;

    fuel.integrated += fuel_per_interval / 360;
    fuel.last_diff = fuel_per_h;

    integrated_metrics[__INTEGRATE_VAR_INTEGRATE_PERIOD_INJECTION] = ms;
    integrated_metrics[__INTEGRATE_VAR_CONS_MOMENT_TIME] = fuel_per_h;
    integrated_metrics[__INTEGRATE_VAR_CONS_MOMENT_DIST] = cons_dist;
}


/*
    например, при времени открытия 1 мс = (250 * 4) и оборотах 2400 = (9600 / 4) мы имеем время оборота (60 * 1000 / 2400) = 25 мс
    за оборот форсунка открывается на 2х цилиндрах, значит отношение будет 2 / 25 = 0.08,
    теперь если мы умножим 0.08 на 100 мс интервала между опросами то получим 8 мс открытого времени всеми форсунками за интервал
    при этом 250 * 9600 = 2400000

    например 4 мс при 4800 оборотах дадут нам 8 мс / 12.5 мс = 0.64 и 1000 * 4 * 4800 = 19200000 * 0.64 = 12288000
*/
