#include "honda_dlc_units.h"

const char * honda_dlc_unit_name(honda_unit_t unit)
{
    #define __HONDA_UNIT_NAME(name, ...) #name
    static const char * honda_uints_name[] = {
        __HONDA_UNITS_DESC(__HONDA_UNIT_NAME)
    };
    return honda_uints_name[unit];
}

#define __HONDA_UNIT_ADDRESS(name, addr, ...) addr
static const uint8_t honda_units_address[] = {
    __HONDA_UNITS_DESC(__HONDA_UNIT_ADDRESS)
};

uint8_t honda_dlc_unit_address(honda_unit_t unit)
{
    return honda_units_address[unit];
}

unsigned honda_dlc_unit_from_address(uint8_t addr, honda_unit_t * unit)
{
    for (unsigned i = 0; i < HONDA_UNIT_COUNT; i++) {
        if (honda_units_address[i] == addr) {
            *unit = i;
            return 1;
        }
    }
    return 0;
}

unsigned honda_dlc_unit_len(honda_unit_t unit)
{
    #define __HONDA_UNIT_MAX_ADDRESS(name, addr, len) len
    static const unsigned honda_units_len[] = {
        __HONDA_UNITS_DESC(__HONDA_UNIT_MAX_ADDRESS)
    };
    return honda_units_len[unit];
}
