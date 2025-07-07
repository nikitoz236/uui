#pragma once

#define METRIC_ADC_VAR_LIST(macro) \
    macro(VOLTAGE_ADC,      "VOLTAGE",      "V",    X100,   2   ), \
    macro(VOLTAGE_TANK,     "TANK SENSOR",  "V",    X100,   2   )

unsigned metric_adc_get_raw(unsigned id);
int metric_adc_get_real(unsigned id);
