#pragma once

#define METRIC_NAME_MAX_LEN sizeof("CHECK ENGINE")
#define METRIC_UNIT_MAX_LEN sizeof("KPa")

//   012345678901234
#define METRIC_ECU_BOOL_LIST(macro) \
    macro(STARTER,          "STARTER",      0, ), \
    macro(AC_BUTTON,        "AC BUTTON",    0, ), \
    macro(POWER_STEERING,   "POWER STEER.", 0, ), \
    macro(BRAKES,           "BRAKES",       0, ), \
    macro(VTEC_SENSOR,      "VTEC SENSOR",  0, ), \
    macro(SCS,              "SCS",          0, ), \
    macro(VTEC_CTRL,        "VTEC CONTROL", 0, ), \
    macro(MAIN_RELAY,       "MAIN RELAY",   0, ), \
    macro(AC_CTRL,          "AC CONTROL",   0, ), \
    macro(O2_HEAT1,         "O2 HEAT 1",    0, ), \
    macro(CHECK_ENGINE,     "CHECK ENGINE", 0, ), \
    macro(O2_HEAT2,         "O2 HEAT 2",    0, ), \
    macro(ALTERNATOR_CTRL,  "ALTER. CTRL",  0, ), \
    macro(FAN_CTRL,         "FAN CONTROL",  0, ), \
    macro(INTAKE_CTRL,      "INTAKE CTRL",  0, ), \
    macro(VTEC_E,           "VTEC E",       0, ), \
    macro(ECONO,            "ECONO",        0, ), \
    macro(AT_CONTOL,        "AT CONTOL",    0, ), \
    macro(CLOSED_LOOP,      "CLOSED LOOP",  0, )

#define METRIC_ECU_VAR_LIST(macro) \
    macro(RPM,              "RPM",          0,      X1,     ), \
    macro(ECU_SPEED,        "SPEED ECU",    "KMh",  X1,     ), \
    macro(ENGINE_T,         "ENGINE t",     "*C",   X1000,  ), \
    macro(INTAKE_T,         "INTAKE t",     "*C",   X1000,  ), \
    macro(INTAKE_P,         "INTAKE P",     "KPa",  X1000,  ), \
    macro(ATM_P,            "ATM P",        "KPa",  X1000,  ), \
    macro(THROTTLE,         "THROTTLE",     "%",    X1000,  ), \
    macro(OXY_1,            "OXY 1",        "V",    X1000,  ), \
    macro(VOLTAGE_ECU,      "VOLTAGE ECU",  "V",    X1000,  ), \
    macro(ALTERNATOR_LOAD,  "ALTERN. LOAD", "%",    X1000,  ), \
    macro(CURRENT,          "CURRENT",      "A",    X1000,  ), \
    macro(ERG_VALVE_V,      "ERG VALVE",    "V",    X1,     ), \
    macro(CORR_SHORT,       "CORR. SHORT",  "%",    X1000,  ), \
    macro(CORR_LONG,        "CORR. LONG",   "%",    X1000,  ), \
    macro(INJECTION,        "INJECTION",    "MS",   X1000,  ), \
    macro(IDLING_ANGLE,     "IGN. ANGLE",   0,      X1000,  ), \
    macro(IDLING_LIMIT,     "IGN. LIMIT",   0,      X1000,  ), \
    macro(AIR_VALVE_POS,    "AIR VALVE",    "%",    X1000,  ), \
    macro(AIR_VALVE_CUR,    "AIR VALVE",    "mA",   X1,     ), \
    macro(ERG_CTRL,         "ERG CONTROL",  "%",    X1000,  ), \
    macro(ERG_VALVE_POS,    "ERG VALVE",    "%",    X1000,  ), \
    macro(AIR_FUEL,         "AIR/FUEL",     0,      X1,     ), \
    macro(PCS_EVAP,         "PCS EVAP",     "%",    X1000,  ), \
    macro(DETONATION,       "DETONATION",   0,      X1,     ), \
    macro(FUEL_SYSTEM,      "FUEL SYSTEM",  0,      X1,     ), \
    macro(ENGINE_LOAD,      "ENGINE LOAD",  "%",    X1000,  ), \
    macro(OXY_2,            "OXY 2",        "V",    X1,     ), \
    macro(MISF_CYL,         "MisF_Cyl",     0,      X1,     ), \
    macro(CKP_LEARN,        "CKP Learn",    0,      X1,     )

#define METRIC_ADC_VAR_LIST(macro) \
    macro(VOLTAGE_ADC,      "VOLTAGE",      "V",    X100    ), \
    macro(VOLTAGE_TANK,     "TANK SENSOR",  "V",    X100    )

#define METRIC_CALC_VAR_LIST(macro) \
    macro(OBD_PERIOD,       "OBD PERIOD",   "ms",   X1,     ), \
    macro(INJECTOR_LOAD,    "INJ. LOAD",    "%",    X1000,  ), \
    macro(TEMP_OUTSIDE,     "OUTSIDE t",    "*C",   X100,   ), \
    macro(TEMP_INTERNAL,    "INTERNAL t",   "*C",   X100,   )





















