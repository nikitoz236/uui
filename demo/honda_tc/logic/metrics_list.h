#pragma once

#define METRIC_ADC_VAR_LIST(macro) \
    macro(VOLTAGE_ADC,      "VOLTAGE",      "V",    X100,   2   ), \
    macro(VOLTAGE_TANK,     "TANK SENSOR",  "V",    X100,   2   )

#define METRIC_CALC_VAR_LIST(macro) \
    macro(INJECTOR_LOAD,    "INJ. LOAD",    "%",    X1000,  2   ), \
    macro(VOLTAGE_MIN,      "MIN VOLTAGE",  "V",    X100,   2   )

#define METRIC_TEMP_VAR_LIST(macro) \
    macro(TEMP_OUTSIDE,     "OUTSIDE t",    "*C",   X100,   1   ), \
    macro(TEMP_INTERNAL,    "INTERNAL t",   "*C",   X100,   1   )
