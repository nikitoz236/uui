#include "stdio.h"
#include "metrics_view.h"

int main()
{
    for (unsigned m_id = 0; m_id < METRIC_ID_NUM; m_id++) {
        printf("metric id %d, name %s value %d", m_id, metric_get_name(m_id), metric_get_val(m_id));
        const char * unit = metric_get_unit(m_id);
        if (unit) {
            printf(" %s", unit);
        }
        printf("\n");
    }
    return 0;
}
