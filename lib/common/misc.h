#pragma once
#include <stdint.h>

#define DIV_ROUND_UP(x, d)                      ((x + (d - 1)) / d)

static inline unsigned round_up_deg2(unsigned val, unsigned step)
{
    return (val + (step - 1)) & ~(step - 1);
}
