#include "metrics_ecu.h"
#include "str_utils.h"

#define FIX_POINT_BITS                  14

#define ECU_FLAGS_ADDR                  0x08
#define ECU_FLAGS_BYTES                 8

#define METRIC_ENUM_NUM(id, ...)        __ECU_VAR_ ## id

enum {
    METRIC_ECU_VAR_LIST(METRIC_ENUM_NUM),
    METRIC_ECU_VAR_NUM
};

enum {
    METRIC_ECU_BOOL_LIST(METRIC_ENUM_NUM),
    METRIC_ECU_BOOL_NUM
};

typedef struct {
    unsigned mull;
    unsigned sub;
    uint8_t address;
    enum {
        METRIC_TYPE_RAW,        // no conversion
        METRIC_TYPE_RMS,        // (raw * mull) - sub
        METRIC_TYPE_RSM,        // (raw - sub) * mull
        METRIC_TYPE_SRM,        // (sub - raw) * mull
        METRIC_TYPE_TMP,        // temperature
    } type : 4;
    uint8_t len : 4;
} ecu_map_t;

#define METRIC_ECU_MAP(id, name, units, f, a, l, t, m, s) \
    { \
        .address = a, \
        .len = l, \
        .type = METRIC_TYPE_ ## t, \
        .mull = (m * (1 << FIX_POINT_BITS)), \
        .sub = s \
    }

#define METRIC_ECU_BOOL_INDEXES(id, name, index) index

static const ecu_map_t honda_ecu_var_map[] = { METRIC_ECU_VAR_LIST(METRIC_ECU_MAP) };
static const uint8_t honda_metric_bool_map[] = { METRIC_ECU_BOOL_LIST(METRIC_ECU_BOOL_INDEXES) };

uint16_t honda_metric_raw[METRIC_ECU_VAR_NUM] = {};
int honda_metric_real[METRIC_ECU_VAR_NUM] = {};

uint8_t honda_metric_bool[ECU_FLAGS_BYTES] = {};

int metric_ecu_get_real(unsigned id)
{
    if (id >= METRIC_ECU_VAR_NUM) {
        return 0;
    }
    return honda_metric_real[id];
}

unsigned metric_ecu_get_raw(unsigned id)
{
    if (id >= METRIC_ECU_VAR_NUM) {
        return 0;
    }
    return honda_metric_raw[id];
}

unsigned metric_ecu_get_bool(unsigned id)
{
    if (id >= METRIC_ECU_BOOL_NUM) {
        return 0;
    }
    unsigned index = honda_metric_bool_map[id];
    unsigned offset = index >> 3;
    unsigned mask = (1 << (index & (8 - 1)));
    if (honda_metric_bool[offset] & mask) {
        return 1;
    }
}

const ecu_map_t * search_var_map(uint8_t addr, const ecu_map_t * map, unsigned map_len)
{
    unsigned min = 0;
    unsigned max = map_len - 1;
    while (min <= max) {
        unsigned half = (min + max) / 2;
        if (honda_ecu_var_map[half].address == addr) {
            return &honda_ecu_var_map[half];
        }
        if (honda_ecu_var_map[half].address < addr) {
            min = half + 1;
        } else {
            max = half - 1;
        }
    }
    return 0;
}

static unsigned ecm_t_conv(unsigned raw){
    double x = raw;
    double result = (155.04149 - x*3.0414878 + x*x*0.03952185 - x*x*x*0.00029383913 + x*x*x*x*0.0000010792568 - x*x*x*x*x*0.0000000015618437) * 1000;
    unsigned r = result;
    return r;
}

void metric_ecu_data_ready(unsigned addr, const uint8_t * data, unsigned len)
{
    while (len) {
        unsigned var_len = 1;
        if ((addr >= ECU_FLAGS_ADDR) && (addr < (ECU_FLAGS_ADDR + ECU_FLAGS_BYTES))) {
            unsigned bin_offset = addr - ECU_FLAGS_ADDR;
            var_len = ECU_FLAGS_BYTES - bin_offset;
            if (len < var_len) {
                var_len = len;
            }
            str_cp(&honda_metric_bool[bin_offset], data, var_len);
        } else {
            const ecu_map_t * map = search_var_map(addr, honda_ecu_var_map, METRIC_ECU_VAR_NUM);
            if (map) {
                unsigned metric_idx = map - honda_ecu_var_map;
                unsigned raw = data[0];
                if (map->len == 2) {
                    var_len = 2;
                    raw <<= 8;
                    raw += data[1];
                }

                // printf("dlc address: %d map %s : address %d, type %d, len %d, mull %d, sub %d, raw %d\n",
                //     addr,
                //     metric_get_name(metric_ecu_bool_num() + metric_idx),
                //     map->address,
                //     map->type,
                //     map->len,
                //     map->mull,
                //     map->sub,
                //     raw
                // );

                int real;
                switch (map->type) {
                    case METRIC_TYPE_RAW:
                        real = raw;
                        break;
                    case METRIC_TYPE_TMP:
                        real = ecm_t_conv(raw);
                        break;
                    case METRIC_TYPE_RMS:
                        real = raw * map->mull;
                        real >>= FIX_POINT_BITS;
                        real -= map->sub;
                        break;
                    case METRIC_TYPE_RSM:
                        real = raw - map->sub;
                        real *= map->mull;
                        real >>= FIX_POINT_BITS;
                        break;
                    case METRIC_TYPE_SRM:
                        real = raw * map->mull;
                        real >>= FIX_POINT_BITS;
                        real = map->sub - real;
                        break;
                    default:
                        real = 0;
                        break;
                }
                honda_metric_raw[metric_idx] = raw;
                honda_metric_real[metric_idx] = real;

                if (metric_idx == __ECU_VAR_INJECTION) {
                    trip_integrate(honda_metric_raw[__ECU_VAR_RPM], honda_metric_raw[__ECU_VAR_ECU_SPEED], raw);
                }
            }
        }
        len -= var_len;
        data += var_len;
        addr += var_len;
    }
}
