#include "trip_integrate.h"
#include "systick.h"

#define METRIC_ENUM_NUM(id, ...)        __INTEGRATE_VAR_ ## id

enum {
    TRIP_INTEGRATE_VAR_LIST(METRIC_ENUM_NUM),
    TRIP_INTEGRATE_VAR_NUM
};

int main()
{
    init_systick();
    trip_restart();

    for (int i = 0; i < 10; i++) {
        trip_integrate(3400 * 4, 123, 4100 / 4);
        printf("dist %d, fuel %d, cons l/100 %d, cons h %d, period %d\n",
            trip_get_dist_m(),
            trip_get_fuel_ml(),
            trip_integrate_get_real(__INTEGRATE_VAR_CONS_MOMENT_DIST),
            trip_integrate_get_real(__INTEGRATE_VAR_CONS_MOMENT_TIME),
            trip_integrate_get_real(__INTEGRATE_VAR_INTEGRATE_PERIOD)
        );
        usleep(453000);
        }

    return 0;
}