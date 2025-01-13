#include "val_mod.h"

#define OVF_ADD(val, max, min, step) \
    (val) += (step); \
    if ((val) > (max)) { \
        (val) -= ((max) - (min) + 1); \
    }

#define OVF_SUB(val, max, min, step) \
    if ((val) < ((min) + (step))) { \
        (val) += ((max) - (min) + 1 ); \
    } \
    (val) -= (step)

#define LIM_ADD(val, max, step) \
    (val) += (step); \
    if ((val) > (max)) { \
        (val) = (max); \
    }

#define LIM_SUB(val, min, step) \
    if ((val) < ((min) + (step))) { \
        (val) = (min); \
    } else { \
        (val) -= (step); \
    }

unsigned val_mod_unsigned(void * ptr, val_size_t size, val_mod_op_t op, unsigned ovf, unsigned min, unsigned max, unsigned step)
{
    unsigned val = val_ptr_to_usnigned(ptr, size);
    if (ovf) {
        if (op == MOD_OP_ADD) {
            OVF_ADD(val, max, min, step);
        } else {
            OVF_SUB(val, max, min, step);
        }
    } else {
        if (op == MOD_OP_ADD) {
            if (val == max) {
                return 0;
            }
            LIM_ADD(val, max, step);
        } else {
            if (val == min) {
                return 0;
            }
            LIM_SUB(val, min, step);
        }
    }
    val_ptr_from_unsigned(ptr, size, val);
    return 1;
}

unsigned val_mod_signed(void * ptr, val_size_t size, val_mod_op_t op, unsigned ovf, int min, int max, int step)
{
    int val = val_ptr_to_signed(ptr, size);
    if (ovf) {
        if (op == MOD_OP_ADD) {
            OVF_ADD(val, max, min, step);
        } else {
            OVF_SUB(val, max, min, step);
        }
    } else {
        if (op == MOD_OP_ADD) {
            if (val == max) {
                return 0;
            }
            LIM_ADD(val, max, step);
        } else {
            if (val == min) {
                return 0;
            }
            LIM_SUB(val, min, step);
        }
    }
    val_ptr_from_signed(ptr, size, val);
    return 1;
}
