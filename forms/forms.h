#pragma once

/*

    библиотека для работы с формами
    определяет точку на двумерном экране, вектор, прямоугольник имеющий положение и размеры

    измерения всегда упорядочены:
        x, y
        ширина, высота
        горизонтальное, вертикальное

    требует файл coord_type.h с определением типа coord_t

*/

#include "coord_type.h"

typedef union {
    struct {                    // for position
        coord_t x;
        coord_t y;
    };
    struct {                    // for size
        coord_t w;
        coord_t h;
    };
    coord_t ca[2];              // coordinates array
} xy_t;

typedef union {
    struct {
        xy_t p;         // position
        xy_t s;         // size
    };
} form_t;

enum align {
    ALIGN_LEFT_OUTSIDE,
    ALIGN_LEFT_INSIDE,
    ALIGN_CENTER,
    ALIGN_RIGHT_INSIDE,
    ALIGN_RIGHT_OUTSIDE,
    ALIGN_UP_OUTSIDE = ALIGN_LEFT_OUTSIDE,
    ALIGN_UP_INSIDE = ALIGN_LEFT_INSIDE,
    ALIGN_DOWN_INSIDE = ALIGN_RIGHT_INSIDE,
    ALIGN_DOWN_OUTSIDE = ALIGN_RIGHT_OUTSIDE,
};

typedef union {
    struct {
        enum align h;
        enum align v;
    };
    enum align ca[2];
} align_mode_t;

void form_align(form_t * pf, form_t * af, align_mode_t * mode, unsigned offset);
void form_extend(form_t * of, form_t * af);
