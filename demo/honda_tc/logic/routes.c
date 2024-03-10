#include "routes.h"
#include "trip_integrate.h"
#include "rtc.h"
#include "str_utils.h"
#include "storage.h"
#include "trip_history.h"

#define ROUTES_FILE_ID          850

static inline uint16_t route_file_id(route_t route)
{
    return route + ROUTES_FILE_ID;
}

const char * route_name(route_t route)
{
    #define __ROUTE_NAMES(id, name) name
    const char * route_names[] = {
        __ROUTE_DESC(__ROUTE_NAMES)
    };
    return route_names[route];
}

unsigned route_start[ROUTE_TYPE_NUM_SAVED][ROUTE_VAL_LOADABLE] = {};
unsigned trip_start = 0;

unsigned trip_get_value(route_value_t value_type)
{
    switch (value_type) {
        case ROUTE_VALUE_DIST:  return trip_get_dist_m();
        case ROUTE_VALUE_FUEL:  return trip_get_fuel_ml();
        case ROUTE_VALUE_TIME:
            if (trip_start) {
                return rtc_get_time_s() - trip_start;
            }
            return 0;
        case ROUTE_VALUE_SINCE: return trip_start;
        default: return 0;
    }
}

unsigned total_get_value(route_value_t value_type)
{
    unsigned start = route_start[ROUTE_TYPE_TOTAL][value_type];
    switch (value_type) {
        case ROUTE_VALUE_DIST:  return start + trip_get_dist_m();
        case ROUTE_VALUE_FUEL:  return start + trip_get_fuel_ml();
        case ROUTE_VALUE_TIME:  return start + trip_get_value(ROUTE_VALUE_TIME);
        case ROUTE_VALUE_SINCE: return 0; // увы бессмысленно
        default: return 0;
    }
}

unsigned route_get_value(route_t route, route_value_t value_type)
{
    if (value_type == ROUTE_VALUE_CONS_DIST) {
        // ml/m = l/km = 100 l/100km
        unsigned dist = route_get_value(route, ROUTE_VALUE_DIST);
        if (dist) {
            return route_get_value(route, ROUTE_VALUE_FUEL) * 100 / dist;
        }
        return 0;
    }
    if (value_type == ROUTE_VALUE_CONS_TIME) {
        // ml/s = 3600 ml/h
        unsigned time = route_get_value(route, ROUTE_VALUE_TIME);
        if (time) {
            return route_get_value(route, ROUTE_VALUE_FUEL) * 3600 / time;
        }
        return 0;
    }
    if (route == ROUTE_TYPE_TRIP) {
        return trip_get_value(value_type);
    }
    if (route == ROUTE_TYPE_TOTAL) {
        return total_get_value(value_type);
    }
    if (value_type == ROUTE_VALUE_SINCE) {
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

void route_trip_end(void)
{
    save_trip_to_history(trip_start, rtc_get_time_s() - trip_start, trip_get_dist_m(), trip_get_fuel_ml());
    for (unsigned type = 0; type <= ROUTE_VALUE_TIME; type++) {
        route_start[ROUTE_TYPE_TOTAL][type] = total_get_value(type);
    }
    route_save(ROUTE_TYPE_TOTAL);
    trip_start = 0;
    trip_restart();
}

void route_trip_start(void)
{
    trip_start = rtc_get_time_s();
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
    route_start[route][ROUTE_VALUE_SINCE] = rtc_get_time_s();
}

void route_reset(route_t route)
{
    route_reset_counters(route);
    route_save(route);
}

void route_load(void)
{
    for (unsigned i = 0; i < ROUTE_TYPE_NUM_SAVED; i++) {
        unsigned len = 0;
        const void * file_ptr = storage_search_file(route_file_id(i), &len);
        if (file_ptr) {
            str_cp(&route_start[i], file_ptr, sizeof(route_start[0]));
            printf("route %s loaded: dist %d fuel %d time %d since %d\n",
                route_name(i),
                route_start[i][ROUTE_VALUE_DIST],
                route_start[i][ROUTE_VALUE_FUEL],
                route_start[i][ROUTE_VALUE_TIME],
                route_start[i][ROUTE_VALUE_SINCE]
            );
        } else {
            route_reset_counters(i);
        }
    }
}
