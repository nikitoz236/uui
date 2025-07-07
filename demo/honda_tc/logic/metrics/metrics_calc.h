#pragma once

typedef struct {
    unsigned mull;
    unsigned sub;
    enum {
        METRIC_TYPE_RAW,        // no conversion
        METRIC_TYPE_RMS,        // (raw * mull) - sub
        METRIC_TYPE_RSM,        // (raw - sub) * mull
        METRIC_TYPE_SRM,        // (sub - raw) * mull
        METRIC_TYPE_TMP,        // temperature
    } type : 4;
    uint8_t len : 4;
} ecu_map_t;


