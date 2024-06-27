#include <stdint.h>

static inline unsigned bf_get(void * array, unsigned index, unsigned field_len)
{
    uint8_t * p = (uint8_t *)array;
    unsigned mask = (1 << field_len) - 1;
    unsigned array_index = (index * field_len) / 8;
    unsigned field_offset = (index * field_len) % 8;
    unsigned value = p[array_index] >> field_offset;
    value &= mask;
    return value;
}

static inline void bf_set(void * array, unsigned index, unsigned field_len, unsigned value)
{
    uint8_t * p = (uint8_t *)array;
    if (field_len == 1) {
        if (value) {
            p[index / 8] |= (1 << (index % 8));
        } else {
            p[index / 8] &= ~(1 << (index % 8));
        }
    } else {
        unsigned mask = (1 << field_len) - 1;
        unsigned array_index = (index * field_len) / 8;
        unsigned field_offset = (index * field_len) % 8;
        p[array_index] &= ~(mask << field_offset);
        p[array_index] |= (value & mask) << field_offset;
    }
}
