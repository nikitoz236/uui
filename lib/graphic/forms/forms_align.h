#pragma once
#include "forms_edge.h"
#include "forms.h"

typedef struct {
    form_edge_t edge : 1;
    enum {
        ALIGN_I = 0,
        ALIGN_O,
    } side : 1;
    unsigned center : 1;
} __align_od_t;

typedef struct {
    union {
        struct {
            __align_od_t x;
            __align_od_t y;
        };
        __align_od_t ca[DIMENSION_COUNT];
    };
} align_t;

xy_t align_form_pos(const form_t * pf, xy_t s, const align_t * a, const xy_t * margin);

static inline void align_form(const form_t * pf, form_t * af, const align_t * a, xy_t * margin)
{
    af->p = align_form_pos(pf, af->s, a, margin);
}
