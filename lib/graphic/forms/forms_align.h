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

#define ALIGN_LIUI (align_t){ .x = { .edge = EDGE_L, .side = ALIGN_I, .center = 0 }, .y = { .edge = EDGE_U, .side = ALIGN_I, .center = 0 } }
#define ALIGN_RIUI (align_t){ .x = { .edge = EDGE_R, .side = ALIGN_I, .center = 0 }, .y = { .edge = EDGE_U, .side = ALIGN_I, .center = 0 } }
#define ALIGN_LIDI (align_t){ .x = { .edge = EDGE_L, .side = ALIGN_I, .center = 0 }, .y = { .edge = EDGE_D, .side = ALIGN_I, .center = 0 } }
#define ALIGN_RIDI (align_t){ .x = { .edge = EDGE_R, .side = ALIGN_I, .center = 0 }, .y = { .edge = EDGE_D, .side = ALIGN_I, .center = 0 } }

#define ALIGN_LIC  (align_t){ .x = { .edge = EDGE_L, .side = ALIGN_I, .center = 0 }, .y = { .center = 1 } }
#define ALIGN_RIC  (align_t){ .x = { .edge = EDGE_R, .side = ALIGN_I, .center = 0 }, .y = { .center = 1 } }
#define ALIGN_CUI  (align_t){ .x = { .center = 1 }, .y = { .edge = EDGE_U, .side = ALIGN_I, .center = 0 } }
#define ALIGN_CDI  (align_t){ .x = { .center = 1 }, .y = { .edge = EDGE_D, .side = ALIGN_I, .center = 0 } }

#define ALIGN_CC   (align_t){ .x = { .center = 1 }, .y = { .center = 1 } }

xy_t align_form_pos(const form_t * pf, xy_t s, align_t a, xy_t padding);

static inline void align_form(const form_t * pf, form_t * af, align_t a, xy_t padding)
{
    af->p = align_form_pos(pf, af->s, a, padding);
}

xy_t size_add_padding(xy_t size, xy_t padding);
