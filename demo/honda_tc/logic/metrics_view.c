#include "metrics_view.h"
#include "array_size.h"

// походу можно просто иметь один список в котором будет тип получения

#define METRIC_ENUM_NUM(id, ...)        ____ENUM_NUM_ ## id

enum { METRIC_ECU_BOOL_LIST(METRIC_ENUM_NUM), METRIC_ECU_BOOL_NUM };

enum { METRIC_ECU_VAR_LIST(METRIC_ENUM_NUM), METRIC_ECU_VAR_NUM };
enum { TRIP_INTEGRATE_VAR_LIST(METRIC_ENUM_NUM), METRIC_INTEGRATE_VAR_NUM };
enum { METRIC_CALC_VAR_LIST(METRIC_ENUM_NUM), METRIC_CALC_VAR_NUM };
enum { METRIC_ADC_VAR_LIST(METRIC_ENUM_NUM), METRIC_ADC_VAR_NUM };
enum { METRIC_TEMP_VAR_LIST(METRIC_ENUM_NUM), METRIC_TEMP_VAR_NUM };

struct metric_info {
    unsigned len;
    int (*real)(unsigned id);
    unsigned (*raw)(unsigned id);
};

struct metric_info metric_info[] = {
    /* same oder as in metric_var_id_t */
    { METRIC_ECU_VAR_NUM, metric_ecu_get_real, metric_ecu_get_raw },
    { METRIC_INTEGRATE_VAR_NUM, trip_integrate_get_real, 0 },
    { METRIC_CALC_VAR_NUM, 0, 0 },
    { METRIC_ADC_VAR_NUM, 0, 0 },
    { METRIC_TEMP_VAR_NUM, 0, 0 }
};

static const struct metric_info * find_metric_info(metric_var_id_t id)
{
    unsigned idx = 0;
    while (idx < ARRAY_SIZE(metric_info)) {
        if (id < metric_info[idx].len) {
            return &metric_info[idx];
        }
        id -= metric_info[idx].len;
        idx++;
    }
    return 0;
}

unsigned metric_bool_get_val(metric_bool_id_t id)
{
    if (id < METRIC_ECU_BOOL_NUM) {
        return metric_ecu_get_bool(id);
    }
    return 0;
}

int metric_var_get_real(metric_var_id_t id)
{
    const struct metric_info * info = find_metric_info(id);
    if (info) {
        if (info->real) {
            return info->real(id);
        }
    }
    return 0;
}

unsigned metric_var_get_raw(metric_var_id_t id)
{
    const struct metric_info * info = find_metric_info(id);
    if (info) {
        if (info->raw) {
            return info->raw(id);
        }
    }
    return 0;
}

#define METRIC_NAME(id, name, ...) name

const char * metric_bool_get_name(metric_bool_id_t id)
{
    static const char * metric_bool_names[] = {
        METRIC_ECU_BOOL_LIST(METRIC_NAME)
        //
        //
    };
    if (id >= METRIC_BOOL_ID_NUM) {
        return 0;
    }
    return metric_bool_names[id];
}

const char * metric_var_get_name(metric_var_id_t id)
{
    static const char * metric_var_names[] = {
        METRIC_VAR_LIST(METRIC_NAME)
    };
    if (id >= METRIC_VAR_ID_NUM) {
        return 0;
    }
    return metric_var_names[id];
}

const char * metric_var_get_unit(metric_var_id_t id)
{
    // может возвращать 0! так как не у всех значений есть единицы измерения
    #define METRIC_UNIT(id, name, unit, ...) unit
    static const char * metric_var_units[] = {
        METRIC_VAR_LIST(METRIC_UNIT)
    };
    if (id >= METRIC_VAR_ID_NUM) {
        return 0;
    }
    return metric_var_units[id];
}

void metric_var_get_factor_point(metric_var_id_t id, dec_factor_t * f, unsigned * point)
{
    #define METRIC_FORMAT(id, name, units, f, p, ... ) { .factor = f, .point = p }

    struct fp {
        dec_factor_t factor : 4;
        uint8_t point : 4;
    };

    static const struct fp metric_var_fp[] = {
        METRIC_VAR_LIST(METRIC_FORMAT)
    };
    if (id >= METRIC_VAR_ID_NUM) {
        return;
    }
    *f = metric_var_fp[id].factor;
    *point = metric_var_fp[id].point;
}
