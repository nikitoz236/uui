#include <stdio.h>

#include "routes.h"
#include "emu_storage.h"
#include "storage.h"


unsigned trip_get_fuel_ml(void)
{
    return 0;
}

unsigned trip_get_dist_m(void)
{
    return 0;
}

void trip_restart(void)
{

}

int main()
{
    printf("routes load test\r\n");

    emu_storage_load();

    storage_init();
    storage_print_info();

    route_load();

    printf("\n\n");

    for (unsigned r = 0; r < ROUTE_TYPE_NUM; r++) {
        printf("route %12s    dist %10d    fuel %10d    time %10d    since t %10d    since odo %10d\r\n",
            route_name(r),
            route_get_value(r, ROUTE_VALUE_DIST),
            route_get_value(r, ROUTE_VALUE_FUEL),
            route_get_value(r, ROUTE_VALUE_TIME),
            route_get_value(r, ROUTE_VALUE_SINCE_TIME),
            route_get_value(r, ROUTE_VALUE_SINCE_ODO)
            // route_get_value(r, ROUTE_VALUE_AVG_SPEED),
            // route_get_value(r, ROUTE_VALUE_CONS_DIST),
            // route_get_value(r, ROUTE_VALUE_CONS_TIME),
        );

    }

    return 0;
}
