#include "honda_units.h"

const char * honda_poll_unit_name(honda_unit_t unit)
{
    static const char * honda_uints_name[] = {
        [HONDA_UNIT_ECU] = "ECU",
        [HONDA_UNIT_ABS] = "ABS",
        [HONDA_UNIT_SRS] = "SRS",
    };

    return honda_uints_name[unit];
}
