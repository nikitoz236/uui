#pragma once

#define METRIC_VOLTAGE_MIN_VAR_LIST(macro) \
    macro(VOLTAGE_MIN,      "MIN VOLTAGE",  "V",    X100,   2   )

int metric_voltage_min_get(unsigned id);

void metric_voltage_min_reset(void);
void metric_voltage_min_process(void);
