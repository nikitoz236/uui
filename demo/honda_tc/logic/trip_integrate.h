#pragma once

void trip_integrate(unsigned rpm, unsigned speed, unsigned injection_time);

unsigned trip_get_fuel_ml(void);
unsigned trip_get_dist_m(void);

void trip_restart(void);

int trip_integrate_get_real(unsigned id);

#define TRIP_INTEGRATE_VAR_LIST(macro) \
    macro(CONS_MOMENT_TIME, "FUEL CONS D",  "L/H",      X1000 ), \
    macro(CONS_MOMENT_DIST, "FUEL CONS H",  "L/100",    X1000 ), \
    macro(INTEGRATE_PERIOD, "OBD PERIOD",   "MS",       X1000 )
