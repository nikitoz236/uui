#include "routes.h"
#include "trip_integrate.h"
#include "rtc.h"
#include "str_utils.h"
#include "storage.h"

#include "dp.h"

/*
    план:
        добаивить параметры, состояние одометра, средняя скорость

        логика что total считает от 0 - с начала момента использования компьютера

        файл настройкой - стартовое состояние одометра

        коррекция по одометру - файл с коэффициентом на который домножаются дистанции чтобы соответствовать одометру на панели приборов

        истории
            поездки
            заправки
            замены масла ?

            для каждой много записей, допустим 20
*/

#define ROUTES_FILE_ID              800

#define TRIP_HISTORY_FILE_ID        840
#define TRIP_HISTORY_LAST_SLOT      (TRIP_HISTORY_RECORDS - 1)

unsigned route_start[ROUTE_TYPE_NUM_SAVED][ROUTE_VAL_LOADABLE] = {};
unsigned trip_start_s = 0;

static uint8_t trip_history_last_index = 0;

static inline uint16_t route_file_id(route_t route)
{
    return route + ROUTES_FILE_ID;
}

const char * route_name(route_t route)
{
    #define __ROUTE_NAMES(id, name) name
    static const char * route_names[] = {
        __ROUTE_DESC(__ROUTE_NAMES)
    };
    return route_names[route];
}

static unsigned trip_get_value(route_value_t value_type)
{
    switch (value_type) {
        case ROUTE_VALUE_DIST:  return trip_get_dist_m();
        case ROUTE_VALUE_FUEL:  return trip_get_fuel_ml();
        case ROUTE_VALUE_TIME:
            if (trip_start_s) {
                return rtc_get_time_s() - trip_start_s;
            }
            return 0;
        case ROUTE_VALUE_SINCE_TIME: return trip_start_s;
        default: return 0;
    }
}

static unsigned total_get_value(route_value_t value_type)
{
    if (value_type == ROUTE_VALUE_SINCE_TIME) {
        return 0;
    }
    unsigned start = route_start[ROUTE_TYPE_TOTAL][value_type];
    return start + trip_get_value(value_type);
}

static unsigned calc_cons_dist(unsigned dist, unsigned fuel)
{
    // ml/m = l/km = 100 l/100km = 100 000 ml/100km
    if (dist) {
        if (fuel < 40000) {
            // 2**32 / 100000 = 42949
            // то есть топливо до 40 литров можно умножить на 100000 и не потерять точность
            return fuel * 100000 / dist;
        } else {
            fuel *= 100;
            if (dist > 10000) {
                // 10 км мы хотя бы можем поделить на 1000
                dist /= 1000;
                fuel /= dist;
            } else if (dist > 100) {
                // 0.1 км можно поделить на 10
                dist /= 10;
                fuel /= dist;
                fuel *= 100;
            } else {
                // наименее точно. просто умножаем результат деления на 1000
                fuel /= dist;
                fuel *= 1000;
            }
            return fuel;
        }
    }
    return 0;
}

static unsigned calc_cons_time(unsigned time, unsigned fuel)
{
    // ml/s = 3600 ml/h
    if (time) {
        return fuel * 3600 / time;
    }
    return 0;
}

unsigned route_get_value(route_t route, route_value_t value_type)
{
    if (value_type == ROUTE_VALUE_CONS_DIST) {
        unsigned dist = route_get_value(route, ROUTE_VALUE_DIST);
        unsigned fuel = route_get_value(route, ROUTE_VALUE_FUEL);
        return calc_cons_dist(dist, fuel);
    }
    if (value_type == ROUTE_VALUE_CONS_TIME) {
        unsigned time = route_get_value(route, ROUTE_VALUE_TIME);
        unsigned fuel = route_get_value(route, ROUTE_VALUE_FUEL);
        return calc_cons_time(time, fuel);
    }
    // if (value_type == ROUTE_VALUE_AVG_SPEED) {

    // }
    if (value_type == ROUTE_VALUE_SINCE_ODO) {
        return route_start[route][ROUTE_VALUE_DIST];
    }
    if (route == ROUTE_TYPE_TRIP) {
        return trip_get_value(value_type);
    }
    if (route == ROUTE_TYPE_TOTAL) {
        return total_get_value(value_type);
    }
    if (value_type == ROUTE_VALUE_SINCE_TIME) {
        // ROUTE_TYPE_TRIP и ROUTE_TYPE_TOTAL обрабатывают SINCE сами отдельно
        return route_start[route][value_type];
    }
    return total_get_value(value_type) - route_start[route][value_type];
}

static void route_save(route_t route)
{
    if (route >= ROUTE_TYPE_NUM_SAVED) {
        return;
    }
    storage_write_file(route_file_id(route), &route_start[route], sizeof(route_start[0]));
}

static file_id_t trip_history_file_id(unsigned slot)
{
    unsigned file_idx = trip_history_last_index;
    if (file_idx < slot) {
        file_idx += TRIP_HISTORY_RECORDS;
    }
    file_idx -= slot;
    return TRIP_HISTORY_FILE_ID + file_idx;
}

unsigned trip_history_get_value(unsigned slot, route_value_t value_type)
{
    if (slot >= TRIP_HISTORY_RECORDS) {
        return 0;
    }
    unsigned len = 0;
    const unsigned * history_data = storage_search_file(trip_history_file_id(slot), &len);
    if (history_data) {
        switch (value_type) {
            case ROUTE_VALUE_CONS_DIST:
                return calc_cons_dist(history_data[ROUTE_VALUE_DIST], history_data[ROUTE_VALUE_FUEL]);

            case ROUTE_VALUE_CONS_TIME:
                return calc_cons_time(history_data[ROUTE_VALUE_TIME], history_data[ROUTE_VALUE_FUEL]);

            default:
                return history_data[value_type];
        }
    }
    return 0;
}

static void save_trip_to_history(void)
{
    unsigned trip_data[ROUTE_VAL_LOADABLE];
    for (unsigned type = 0; type < ROUTE_VAL_LOADABLE; type++) {
        trip_data[type] = trip_get_value(type);
    }
    file_id_t file_id = trip_history_file_id(TRIP_HISTORY_LAST_SLOT);
    trip_history_last_index++;
    if (trip_history_last_index >= TRIP_HISTORY_RECORDS) {
        trip_history_last_index = 0;
    }
    dp("save trip to history to file "); dpd(file_id, 5); dn();
    storage_write_file(file_id, trip_data, sizeof(trip_data));
}

void route_trip_end(void)
{
    save_trip_to_history();
    for (unsigned type = 0; type <= ROUTE_VALUE_TIME; type++) {
        route_start[ROUTE_TYPE_TOTAL][type] = total_get_value(type);
    }
    route_save(ROUTE_TYPE_TOTAL);
    trip_start_s = 0;
    trip_restart();
}

void route_trip_start(void)
{
    trip_start_s = rtc_get_time_s();
}

static void route_reset_counters(route_t route)
{
    if (route == ROUTE_TYPE_TOTAL) {
        return;
    }
    if (route == ROUTE_TYPE_TRIP) {
        return;
    }
    for (unsigned type = 0; type <= ROUTE_VALUE_TIME; type++) {
        route_start[route][type] = total_get_value(type);
    }
    route_start[route][ROUTE_VALUE_SINCE_TIME] = rtc_get_time_s();
}

void route_reset(route_t route)
{
    route_reset_counters(route);
    route_save(route);
}

static void route_starts_load(void)
{
    dpn("\nroute load");
    for (unsigned i = 0; i < ROUTE_TYPE_NUM_SAVED; i++) {
        unsigned len = 0;
        const void * file_ptr = storage_search_file(route_file_id(i), &len);
        if (file_ptr) {
            str_cp(&route_start[i], file_ptr, sizeof(route_start[0]));
            dp("  route "); dpl(route_name(i), 12); dp(" loaded. dist: "); dpd(route_start[i][ROUTE_VALUE_DIST], 10);
            dp(" fuel: "); dpd(route_start[i][ROUTE_VALUE_FUEL], 10);
            dp(" time: "); dpd(route_start[i][ROUTE_VALUE_TIME], 10);
            dp(" since: "); dpd(route_start[i][ROUTE_VALUE_SINCE_TIME], 10);
            dn();
        } else {
            dp("route "); dpl(route_name(i), 12); dpn(" - file not found");
            route_reset_counters(i);
        }
    }
}

static void trip_history_load(void)
{
    unsigned last_since = 0;
    for (unsigned i = 0; i < TRIP_HISTORY_RECORDS; i++) {
        unsigned len = 0;
        const unsigned * history_data = storage_search_file(TRIP_HISTORY_FILE_ID + i, &len);
        if (history_data) {
            if (last_since < history_data[ROUTE_VALUE_SINCE_TIME]) {
                last_since = history_data[ROUTE_VALUE_SINCE_TIME];
                trip_history_last_index = i;
            }
        }
    }
    if (last_since == 0) {
        trip_history_last_index = TRIP_HISTORY_LAST_SLOT;
    }
    dp("trip history last index "); dpd(trip_history_last_index, 2); dn();
}

void route_load(void)
{
    route_starts_load();
    trip_history_load();
}
