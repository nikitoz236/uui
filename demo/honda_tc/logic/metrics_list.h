#pragma once

#define METRIC_INJECTOR_LOAD_VAR_LIST(macro) \
    macro(INJECTOR_LOAD,    "INJ. LOAD",    "%",    X1000,  2   )

#define METRIC_TEMP_VAR_LIST(macro) \
    macro(TEMP_OUTSIDE,     "OUTSIDE t",    "*C",   X100,   1   ), \
    macro(TEMP_INTERNAL,    "INTERNAL t",   "*C",   X100,   1   )
