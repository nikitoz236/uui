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

    return 0;
}
