#include "metrics_view.h"


#define METRIC_ENUM_NUM(id, ...)         ____ENUM_NUM_ ## id

enum {
    METRIC_ECU_BOOL_LIST(METRIC_ENUM_NUM),
    METRIC_ECU_BOOL_NUM
};

enum {
    METRIC_ECU_VAR_LIST(METRIC_ENUM_NUM),
    METRIC_ECU_VAR_NUM
};

unsigned metric_get_val(metric_id_t metric)
{
    if (metric < METRIC_ECU_BOOL_NUM) {
        return metric & 1;
    }
    return metric * 12;
}

const char * metric_get_name(metric_id_t metric)
{
    #define METRIC_NAME(id, name, ...) name
    static const char * metric_names[] = {
        METRIC_ECU_BOOL_LIST(METRIC_NAME),
        METRIC_ECU_VAR_LIST(METRIC_NAME),
        METRIC_ADC_VAR_LIST(METRIC_NAME),
        METRIC_CALC_VAR_LIST(METRIC_NAME)
    };
    return metric_names[metric];
}

const char * metric_get_unit(metric_id_t metric)
{
    #define METRIC_UNIT(id, name, unit, ...) unit
    static const char * metric_units[] = {
        METRIC_ECU_VAR_LIST(METRIC_UNIT),
        METRIC_ADC_VAR_LIST(METRIC_UNIT),
        METRIC_CALC_VAR_LIST(METRIC_UNIT)
    };
    if (metric < METRIC_ECU_BOOL_NUM) {
        return 0;
    }
    return metric_units[metric - METRIC_ECU_BOOL_NUM];
}

unsigned metric_bool_num(void)
{
    return METRIC_ECU_BOOL_NUM;
}
