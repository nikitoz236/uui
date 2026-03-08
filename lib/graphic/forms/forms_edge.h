#pragma once
#include <stdint.h>

typedef enum {
    EDGE_U,
    EDGE_D,
    EDGE_L = EDGE_U,
    EDGE_R = EDGE_D,
    EDGE_COUNT
} form_edge_t;
