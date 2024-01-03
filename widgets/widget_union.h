#pragma once
#include "widgets.h"

extern widget_t __widget_union;

typedef struct {
    ui_node_t nodes[2];
    align_mode_t align_mode;
} widget_union_cfg_t;

#define WIDGET_UNION(hm, ho, vm, vo, n1, n2)  \
    { \
        .widget = &__widget_union, \
        .widget_cfg = &(widget_union_cfg_t){ \
            .nodes = { n1, n2 }, \
            .align_mode = { \
                .h = { \
                    .mode = ALIGN_ ## hm, \
                    .offset = ho \
                }, \
                .v = { \
                    .mode = ALIGN_ ## vm, \
                    .offset = vo \
                } \
            } \
        } \
    }
