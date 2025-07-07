#include "adc_hw.h"
#include "metrics_adc.h"

unsigned metric_adc_get_raw(unsigned id)
{
    return adc_get(id);
}

int metric_adc_get_real(unsigned id)
{
    /*
        R UP = 52K3
        R DOWN = 10K
        ADC MAX = 65535
        ADC VREF = 3V3
        FACTOR = 100

        VOLTAGE = ADC * VREF * (R UP + R DOWN) * FACTOR / (R DOWN * ADC MAX)
    */

    #define R_UP        52300
    #define R_DOWN      10000
    #define ADC_MAX     65535
    #define VREF        3.3
    #define FACTOR      100

    #define MUL         VREF * (R_UP + R_DOWN) / 10             // 20559
    #define DIV         (R_DOWN * ADC_MAX) / FACTOR / 10        // 655350

    int real = adc_get(id);
    real *= MUL;
    real /= DIV;
    return real;
}
