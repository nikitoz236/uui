#pragma once
#include <stdint.h>

#define __ROUTE_DESC(macro) \
    /*    name     */ \
    /* TOTAL должен загружаться первым */  \
    macro(TOTAL,        "TOTAL"),           /* вообще за все время эксплуатации */ \
    macro(TRAVEL,       "TRAVEL"),          /* поездка (из города в город) */ \
    macro(JOURNEY,      "JOURNEY"),         /* путешествие много дней разные города*/ \
    macro(TMP,          "TMP"),             /* просто засечь */ \
    macro(DAY,          "DAY"),             /* за сегодняшний день с полуночи */ \
    macro(OIL_CHANGE,   "OIL CHANGE"),      /* с замены масла */ \
    macro(ENGINE_MAINT, "ENGINE MT"),       /* с обслуживания двигателя */ \
    macro(REFILL,       "REFILL"),          /* с момента заправки */ \
    /* TRIP не сохраняется, по этому должен быть последним */ \
    macro(TRIP,         "TRIP")                             /* с момента запуска двигателя */

#define __ROUTE_ENUM(x, ...) ROUTE_TYPE_ ## x

typedef enum {
    __ROUTE_DESC(__ROUTE_ENUM),
    ROUTE_TYPE_NUM,
    ROUTE_TYPE_NUM_SAVED = ROUTE_TYPE_TRIP,
} route_t;

/*
    добавить возможность посмотреть скок было на одометре когда началось. интересно зачем ?
    история по поездкам и по заправкам

    отдельный маршрут типа TOTAL но чтобы моточасы с километрами нормально бились, просто обычный еще какойто
*/


typedef enum {
    ROUTE_VALUE_DIST,       // расстояние в метрах
    ROUTE_VALUE_FUEL,       // израсходованное топливо в мл
    ROUTE_VALUE_TIME,       // моточасы в секундах
    ROUTE_VALUE_SINCE_TIME,           // момент времени начала в секундах

    // параметры выше сохраняются как файл маршрута через ROUTE_VAL_LOADABLE

    ROUTE_VALUE_SINCE_ODO,       // значение одометра в момент начала
    // ROUTE_VALUE_MOV_TIME         // время в движении (ненулевая скорость)
    // ROUTE_VALUE_AVG_SPEED,       // средняя скорость за маршрут
            // нужно считать время в движении

    ROUTE_VALUE_CONS_DIST,  // расход топлива в мл / 100 км
    ROUTE_VALUE_CONS_TIME,  // расход топлива в мл / час
    ROUTE_VALUE_NUM,
    ROUTE_VAL_LOADABLE = ROUTE_VALUE_SINCE_TIME + 1
} route_value_t;

const char * route_name(route_t route);
unsigned route_get_value(route_t route, route_value_t value_type);

// сброс маршрута - сохранение в файловой системе файла со стартовыми значениями
void route_reset(route_t route);

// инициализация - чтение маршрутов и истории из файловой системы
void route_load(void);

// сохранение маршрута trip в файловую систему и историю, дергается при остановке двигателя
void route_trip_end(void);

// сброс маршрута trip, дергается при запуске двигателя
void route_trip_start(void);


#define TRIP_HISTORY_RECORDS        20
// slot 0 - последний, slot 1 - предпоследний, (TRIP_HISTORY_RECORDS - 1) - самый старый
unsigned trip_history_get_value(unsigned slot, route_value_t value_type);
