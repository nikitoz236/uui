#pragma once

#define METRIC_ADC_VAR_LIST(macro) \
    macro(VOLTAGE_ADC,      "VOLTAGE",      "V",    X100    ), \
    macro(VOLTAGE_TANK,     "TANK SENSOR",  "V",    X100    )

#define METRIC_CALC_VAR_LIST(macro) \
    macro(OBD_PERIOD,       "OBD PERIOD",   "ms",   X1,     ), \
    macro(INJECTOR_LOAD,    "INJ. LOAD",    "%",    X1000,  )

#define METRIC_TEMP_VAR_LIST(macro) \
    macro(TEMP_OUTSIDE,     "OUTSIDE t",    "*C",   X100,   ), \
    macro(TEMP_INTERNAL,    "INTERNAL t",   "*C",   X100,   )
