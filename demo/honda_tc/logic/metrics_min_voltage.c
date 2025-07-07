#include "metrics_view.h"

static int min_voltage = 0;

int metric_voltage_min_get(unsigned id)
{
    (void) id;
    return min_voltage;
}

void metric_voltage_min_reset(void)
{
    min_voltage = metric_var_get_real(METRIC_ID_VOLTAGE_ADC);
}

void metric_voltage_min_process(void)
{
    int voltage = metric_var_get_real(METRIC_ID_VOLTAGE_ADC);
    if (voltage < min_voltage) {
        min_voltage = voltage;
    }
}
