#pragma once
#include <stdint.h>

#define __HONDA_UNITS_DESC(m) \
    m(ECU, 0x20, 0x100), \
    m(ABS, 0xA0, 0x80), \
    m(SRS, 0x60, 0x80)

#define __HONDA_UNIT_ENUM(name, ...) HONDA_UNIT_ ## name

typedef enum {
    __HONDA_UNITS_DESC(__HONDA_UNIT_ENUM),

    HONDA_UNIT_COUNT
} honda_unit_t;

const char * honda_dlc_unit_name(honda_unit_t unit);
uint8_t honda_dlc_unit_address(honda_unit_t unit);
unsigned honda_dlc_unit_len(honda_unit_t unit);
