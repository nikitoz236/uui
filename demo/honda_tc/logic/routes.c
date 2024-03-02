#include "routes.h"
#include "trip_integrate.h"
#include "rtc.h"
#include "str_utils.h"
#include "flash_fs.h"

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

unsigned route_start[ROUTE_NUM_SAVED][ROUTE_VALUE_SINCE] = {};
unsigned trip_start = 0;

unsigned trip_get_value(route_value_t value_type)
{
    switch (value_type) {
        case ROUTE_VALUE_DIST:  return trip_get_dist_m();
        case ROUTE_VALUE_FUEL:  return trip_get_fuel_ml();
        case ROUTE_VALUE_TIME:  return rtc_get_time_s() - trip_start;
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
        return route_get_value(route, ROUTE_VALUE_FUEL) * 100 / route_get_value(route, ROUTE_VALUE_DIST);
    }
    if (value_type == ROUTE_VALUE_CONS_TIME) {
        // ml/s = 3600 ml/h
        return route_get_value(route, ROUTE_VALUE_FUEL) * 3600 / route_get_value(route, ROUTE_VALUE_TIME);
    }
    if (route == ROUTE_TYPE_TRIP) {
        return trip_get_value(value_type);
    }
    if (route == ROUTE_TYPE_TOTAL) {
        return total_get_value(value_type);
    }
    if (value_type == ROUTE_VALUE_SINCE) {
        return route_start[route][value_type];
    }
    return total_get_value(value_type) - route_start[route][value_type];
}

static void route_save(route_t route)
{
    flash_fs_file_write(route_file_id(route), &route_start[route], sizeof(route_start[0]));
}

void route_reset(route_t route)
{
    if (route == ROUTE_TYPE_TOTAL) {
        return;
    }
    if (route == ROUTE_TYPE_TRIP) {
        trip_start = rtc_get_time_s();
        trip_reset();
        return;
    }
    route_start[route][ROUTE_VALUE_SINCE] = rtc_get_time_s();
    route_start[route][ROUTE_VALUE_DIST] = trip_get_dist_m();
    route_start[route][ROUTE_VALUE_FUEL] = trip_get_fuel_ml();
    route_save(route);
}

void route_load(void)
{
    void * file_ptr;
    for (unsigned i = 0; i < ROUTE_NUM_SAVED; i++) {
        unsigned len = flash_fs_file_search(route_file_id(i), &file_ptr);
        if (len == sizeof(route_start[0])) {
            str_cp(file_ptr, &route_start[i], sizeof(route_start[0]));
        } else {
            reset_route(i);
        }
    }
}
