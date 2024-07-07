#pragma once
#include "time_units.h"

typedef struct {
    unsigned ms;
    unsigned systick;
} time_moment_t;

void time_moment_save(time_moment_t * tm);

unsigned time_moment_interval_cycles(time_moment_t * start, time_moment_t * end);
unsigned time_moment_interval_us(time_moment_t * start, time_moment_t * end);
unsigned time_moment_interval_ms(time_moment_t * start, time_moment_t * end);
