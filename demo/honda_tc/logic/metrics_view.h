#pragma once
#include "metrics_list.h"

#define METRIC_ENUM(id, ...)         METRIC_ID_ ## id

typedef enum {
    METRIC_ECU_BOOL_LIST(METRIC_ENUM),
    METRIC_ECU_VAR_LIST(METRIC_ENUM),
    METRIC_ADC_VAR_LIST(METRIC_ENUM),
    METRIC_CALC_VAR_LIST(METRIC_ENUM),

    METRIC_ID_NUM
} metric_id_t;

unsigned metric_bool_num(void);

unsigned metric_get_val(metric_id_t metric);
unsigned metric_get_val(metric_id_t metric);

const char * metric_get_name(metric_id_t metric);
const char * metric_get_unit(metric_id_t metric);


// from poll hw
void dlc_data_ready(honda_unit_t unit, unsigned addr, uint8_t * data, unsigned len);
