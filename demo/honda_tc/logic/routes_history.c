#include "routes.h"

typedef enum {
    ROUTE_HISTORY_TYPE_TRIP,
    ROUTE_HISTORY_TYPE_REFILL,

    ROUTE_HISTORY_TYPE_NUM
} route_history_type_t;

route_t route_history_list[] = {
    [ROUTE_HISTORY_TYPE_TRIP] = ROUTE_TYPE_TRIP,
    [ROUTE_HISTORY_TYPE_REFILL] = ROUTE_TYPE_REFILL,
};

typedef struct {
    uint32_t v[ROUTE_HISTORY_VAL_LOADABLE];
} route_history_file_t;

static uint8_t last_idx[ROUTE_HISTORY_TYPE_NUM] = {};

static file_id_t trip_history_file_id(route_history_type_t t, unsigned slot)
{
    unsigned file_idx = last_idx[t];
    if (file_idx < slot) {
        file_idx += TRIP_HISTORY_RECORDS;
    }
    file_idx -= slot;
    return TRIP_HISTORY_FILE_ID + file_idx;
}

void history_save(route_history_type_t t)
{
    route_t route = route_history_list[t];
    route_history_file_t history_data;
    for (unsigned type = 0; type < ROUTE_HISTORY_VAL_LOADABLE; type++) {
        history_data.v[type] = route_get_value(route, type);
    }
}



