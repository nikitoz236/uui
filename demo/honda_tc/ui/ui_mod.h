#pragma once
#include "val_mod.h"
#include "text_field.h"

typedef struct {
    int min;
    int max;
    int step;
    uint8_t ovf : 1;
} mod_cfg_t;

typedef struct {
    mod_cfg_t cfg;
    const label_t * l;
} uv_mod_t;

typedef struct {
    const uv_mod_t * mod_list;
    void (*prepare)(void * ctx);
    void (*apply)(void * ctx);
    void (*change)(void * ctx);
    uint16_t offset;
    uint8_t count;
} setting_mod_list_t;

static inline unsigned val_mod(void * ptr, val_rep_t rep, const mod_cfg_t * cfg, val_mod_op_t op)
{
    if (rep.s) {
        return val_mod_signed(ptr, rep.vs, op, cfg->ovf, cfg->min, cfg->max, cfg->step);
    } else {
        return val_mod_unsigned(ptr, rep.vs, op, cfg->ovf, cfg->min, cfg->max, cfg->step);
    }
}
