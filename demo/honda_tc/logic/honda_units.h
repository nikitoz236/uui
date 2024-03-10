#pragma once

typedef enum {
    HONDA_UNIT_ECU,
    HONDA_UNIT_ABS,
    HONDA_UNIT_SRS,

    HONDA_UNIT_COUNT
} honda_unit_t;

const char * honda_poll_unit_name(honda_unit_t unit);
