#pragma once
#include <stdint.h>

/*

    библиотека для работы с геометрией отображаемых элементов интерфейса
    определяет точку на двумерном экране, вектор, прямоугольник имеющий положение и размеры

    измерения всегда упорядочены:
        x, y
        ширина, высота
        горизонтальное, вертикальное

    требует файл coord_type.h с определением типа coord_t - размер переменной достаточный для адресации
    пикселя в одном измерении (для маленьких дисплеев достаточно uint8_t)

*/

#include "coord_type.h"

typedef union {                 // описание двумерного вектора
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
    ALIGN_C = ALIGN_CENTER,
    ALIGN_LI = ALIGN_LEFT_INSIDE,
    ALIGN_LO = ALIGN_LEFT_OUTSIDE,
    ALIGN_RI = ALIGN_RIGHT_INSIDE,
    ALIGN_RO = ALIGN_RIGHT_OUTSIDE,
    ALIGN_UI = ALIGN_UP_INSIDE,
    ALIGN_UO = ALIGN_UP_OUTSIDE,
    ALIGN_DI = ALIGN_DOWN_INSIDE,
    ALIGN_DO = ALIGN_DOWN_OUTSIDE
};

struct __attribute__((__packed__)) __align_cfg {
    enum align mode : 3;
    uint16_t offset : 13;
};

typedef union {
    struct {
        struct __align_cfg h;
        struct __align_cfg v;
    };
    struct __align_cfg ca[2];
} align_mode_t;

#define ALIGN_MODE(hm, ho, vm, vo) \
    { \
        .h = { \
            .mode = ALIGN_ ## hm, \
            .offset = ho \
        }, \
        .v = { \
            .mode = ALIGN_ ## vm, \
            .offset = vo \
        } \
    }

void form_align(form_t * pf, form_t * af, align_mode_t * mode);
void form_union_calc_size(form_t * f1, form_t * f2, align_mode_t * mode, form_t * rf);
void form_union_calc_pos(form_t * of, form_t * f1, form_t * f2, align_mode_t * mode);
