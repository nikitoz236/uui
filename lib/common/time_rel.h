#pragma once

static inline int __time_rel(unsigned t, unsigned ref)
{
    // > 0 when t before ref
    // < 0 when t after ref
    return (int)((int)ref - (int)t);
}
