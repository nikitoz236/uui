#pragma once
#include "val_pack.h"

typedef enum {
    MOD_OP_ADD,
    MOD_OP_SUB,
} val_mod_op_t;

unsigned val_mod_unsigned(void * ptr, val_size_t size, val_mod_op_t op, unsigned ovf, unsigned min, unsigned max, unsigned step);
unsigned val_mod_signed(void * ptr, val_size_t size, val_mod_op_t op, unsigned ovf, int min, int max, int step);
