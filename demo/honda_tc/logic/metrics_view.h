#pragma once
#include <stdint.h>

#include "metrics_ecu.h"
#include "metrics_list.h"

/*
    это модуль обертка для получения данных параметров в виджеты через единый интерфейс

    у нас есть булевые значения, их использует отдельный виджет ? или нет ?


    то есть это агрегатор, он содержит все строки имен и юнитов

        булевый с названием
        параметр с название и юнитами или сырое значение

        маршрут параметр - тип параметр значение единицы
        маршрут полный - тип и все параметры маршрута

        время
        дата

        сырое значение
        координаты

*/

#define METRIC_ENUM(id, ...)         METRIC_ID_ ## id

typedef enum {
    METRIC_ECU_BOOL_LIST(METRIC_ENUM),

    METRIC_BOOL_ID_NUM
} metric_bool_id_t;

unsigned metric_bool_get_val(metric_bool_id_t id);
const char * metric_bool_get_name(metric_bool_id_t id);

#define METRIC_VAR_LIST(macro) \
    METRIC_ECU_VAR_LIST(macro), \
    METRIC_CALC_VAR_LIST(macro), \
    METRIC_ADC_VAR_LIST(macro), \
    METRIC_TEMP_VAR_LIST(macro)

typedef enum {
    METRIC_VAR_LIST(METRIC_ENUM),
    METRIC_VAR_ID_NUM
} metric_var_id_t;

#define METRIC_NAME_MAX_LEN         (sizeof("CHECK ENGINE"))
#define METRIC_UNIT_MAX_LEN         (sizeof("L/100"))

int metric_var_get_real(metric_var_id_t id);
unsigned metric_var_get_raw(metric_var_id_t id);
const char * metric_var_get_name(metric_var_id_t id);
const char * metric_var_get_unit(metric_var_id_t id);
