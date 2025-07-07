
#define FIX_POINT_BITS                  14

#define METRIC_ECU_MAP(id, name, units, f, p, a, l, t, m, s) \
    { \
        .address = a, \
        .len = l, \
        .type = METRIC_TYPE_ ## t, \
        .mull = (m * (1 << FIX_POINT_BITS)), \
        .sub = s \
    }
