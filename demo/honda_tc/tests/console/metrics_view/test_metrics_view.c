#include "stdio.h"
#include "metrics_view.h"

/*
    тест модуля metrics_view.c

    - проверяет все существующие измеряемые и вычисляемые параметры, как они склеиваются в один список, численные и флаговые
    - проверяет что нормально достаются множители и количество знаков после запятой
*/

int metric_ecu_get_real(unsigned id) { return id * 100; };
unsigned metric_ecu_get_raw(unsigned id) { return id * 16; };
unsigned metric_ecu_get_bool(unsigned id) { return id & 1; };

int main()
{
    for (unsigned m_id = 0; m_id < METRIC_VAR_ID_NUM; m_id++) {
        printf("metric id %2d, name %16s     value %10d", m_id, metric_var_get_name(m_id), metric_var_get_real(m_id));
        const char * unit = metric_var_get_unit(m_id);
        if (unit) {
            printf(" %6s", unit);
        } else {
            printf("       ");
        }
        dec_factor_t factor;
        unsigned point;
        metric_var_get_factor_point(m_id, &factor, &point);
        printf("    f %1d, p %1d", factor, point);
        printf("  - %04X\n", metric_var_get_raw(m_id));
    }

    for (unsigned m_id = 0; m_id < METRIC_BOOL_ID_NUM; m_id++) {
        printf("metric id %2d, name %16s     value %1d\n", m_id, metric_bool_get_name(m_id), metric_bool_get_val(m_id));
    }
    return 0;
}
