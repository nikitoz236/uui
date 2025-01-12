#include "honda_dlc_units.h"

const char * honda_dlc_unit_name(honda_unit_t unit)
{
    #define __HONDA_UNIT_NAME(name, ...) #name
    static const char * honda_uints_name[] = {
        __HONDA_UNITS_DESC(__HONDA_UNIT_NAME)
    };
    return honda_uints_name[unit];
}

uint8_t honda_dlc_unit_address(honda_unit_t unit)
{
    #define __HONDA_UNIT_ADDRESS(name, addr, ...) addr
    static const uint8_t honda_units_address[] = {
        __HONDA_UNITS_DESC(__HONDA_UNIT_ADDRESS)
    };
    return honda_units_address[unit];
}

unsigned honda_dlc_unit_len(honda_unit_t unit)
{
    #define __HONDA_UNIT_MAX_ADDRESS(name, addr, len) len
    static const unsigned honda_units_len[] = {
        __HONDA_UNITS_DESC(__HONDA_UNIT_MAX_ADDRESS)
    };
    return honda_units_len[unit];
}
