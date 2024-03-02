#pragma once

void trip_integrate_speed(unsigned val);
void trip_integrate_injectors(unsigned injection_time, unsigned rpm);

unsigned trip_get_fuel(void);
unsigned trip_get_dist(void);

void trip_restart(void);

int trip_integrate_get_real(unsigned id);

#define TRIP_INTEGRATE_VAR_LIST(macro) \
    macro(CONS_MOMENT_DIST, "FUEL CONS H", "L/H"), \
    macro(CONS_MOMENT_TIME, "FUEL CONS D", "L/100"), \
    macro(INTEGRATE_PERIOD_INJECTION, "OBD PERIOD", "MS" )
