#include "metrics_view.h"
#include <stdint.h>
#include "honda_units.h"
#include "array_size.h"
#include "str_utils.h"

#define METRIC_ENUM_NUM(id, ...)         ____ENUM_NUM_ ## id

enum {
    METRIC_ECU_BOOL_LIST(METRIC_ENUM_NUM),
    METRIC_ECU_BOOL_NUM
};

enum {
    METRIC_ECU_VAR_LIST(METRIC_ENUM_NUM),
    METRIC_ECU_VAR_NUM
};

typedef struct {
    unsigned factor;
    unsigned sub;
    uint8_t address;
    enum {
        METRIC_TYPE_RAW,
        METRIC_TYPE_MUL_SUB,
        METRIC_TYPE_SUB_MUL,
        METRIC_TYPE_FROM_SUB_M,
        METRIC_TYPE_TEMP,
    } type : 4;
    uint8_t len : 4;
} ecu_map_t;

#define METRIC_ECU_BOOL_INDEXES(id, name, index) index

#define METRIC_ECU_MAP(id, name, units, factor, a, l, type, m, s) \
    { \
        .address = a, \
        .type = METRIC_TYPE ## type, \
        .len = l, \
        .factor = (m << 16), \
        .sub = s \
    }

static const uint8_t honda_metric_bool_map[] = { METRIC_ECU_BOOL_LIST(METRIC_ECU_BOOL_INDEXES) };
static const ecu_map_t honda_ecu_map[] = { METRIC_ECU_VAR_LIST(METRIC_ECU_MAP) };


uint16_t honda_metric_raw[METRIC_ECU_VAR_NUM] = {};
unsigned honda_metric_real[METRIC_ECU_VAR_NUM] = {};
uint8_t honda_metric_bool[8] = {};


static inline unsigned metric_bool(metric_id_t metric)
{
    unsigned index = honda_metric_bool_map[metric];
    unsigned offset = index >> 3;
    unsigned mask = (1 << (index & (8 - 1)));
    return honda_metric_bool[offset] & mask;
}

unsigned metric_get_val(metric_id_t metric)
{
    if (metric < METRIC_ECU_BOOL_NUM) {
        return metric_bool(metric);
    }
    return honda_metric_real[metric - METRIC_ECU_BOOL_NUM];
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

static unsigned obd_t_conv(unsigned hex){
    double x = hex;
    double result = (155.04149 - x*3.0414878 + x*x*0.03952185 - x*x*x*0.00029383913 + x*x*x*x*0.0000010792568 - x*x*x*x*x*0.0000000015618437) * 1000;
    unsigned r = result;
    return r;
}

ecu_map_t * search_var_map(uint8_t addr, ecu_map_t * map, unsigned map_len)
{
    unsigned min = 0;
    unsigned max = map_len - 1;
    while (min <= max) {
        unsigned half = (min + max) / 2;
        if (honda_ecu_map[half].address == addr) {
            return &honda_ecu_map[half];
        }
        if (honda_ecu_map[half].address < addr) {
            min = half + 1;
        } else {
            max = half - 1;
        }
    }
    return 0;
}

static inline void ecu_data_ready(unsigned addr, uint8_t * data, unsigned len)
{
    while (len) {
        unsigned var_len = 1;
        if ((addr >= 0x08) && (addr <= 0x0F)) {
            unsigned bin_offset = addr - 0x08;
            var_len = 8 - bin_offset;
            if (len < var_len) {
                var_len = len;
            }
            str_cp(&honda_metric_bool[bin_offset], data, var_len);
        } else {
            ecu_map_t * map = search_var_map(addr, honda_ecu_map, ARRAY_SIZE(honda_ecu_map));
            if (map) {
                unsigned metric_idx = map - honda_ecu_map;
                unsigned raw = data[0];
                if (map->len == 2) {
                    var_len = 2;
                    raw <<= 8;
                    raw += data[1];
                }
                honda_metric_raw[metric_idx] = raw;
                int real;
                switch (map->type) {
                    case METRIC_TYPE_RAW:
                        real = raw;
                        break;
                    case METRIC_TYPE_TEMP:
                        real = obd_t_conv(raw);
                        break;
                    case METRIC_TYPE_MUL_SUB:
                        real = raw * map->factor;
                        real >>= 16;
                        real -= map->sub;
                        break;
                    case METRIC_TYPE_SUB_MUL:
                        real = raw - map->sub;
                        real *= map->factor;
                        real >>= 16;
                        break;
                    case METRIC_TYPE_FROM_SUB_M:
                        real = raw * map->factor;
                        real >>= 16;
                        real = map->sub - real;
                        break;
                    default:
                        real = 0;
                        break;
                }
                honda_metric_raw[metric_idx] = raw;
                honda_metric_real[metric_idx] = real;
            }
        }
        len -= var_len;
        data += var_len;
        addr += var_len;
    }
}

void dlc_data_ready(honda_unit_t unit, unsigned addr, uint8_t * data, unsigned len)
{
    if (unit == HONDA_UNIT_ECU) {
        ecu_data_ready(addr, data, len);
    }
}
