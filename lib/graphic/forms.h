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

typedef enum {
    DIMENSION_X,
    DIMENSION_Y,
    DIMENSION_WIDTH = DIMENSION_X,
    DIMENSION_HEIGHT = DIMENSION_Y,

    DIMENSION_COUNT
} dimension_t;

typedef enum {
    EDGE_U,
    EDGE_D,
    EDGE_L = EDGE_U,
    EDGE_R = EDGE_D
} form_edge_t;

typedef union {                 // описание двумерного вектора
    struct {                    // for position
        coord_t x;
        coord_t y;
    };
    struct {                    // for size
        coord_t w;
        coord_t h;
    };
    coord_t ca[DIMENSION_COUNT];              // coordinates array
} xy_t;

typedef struct {
    xy_t p;         // position
    xy_t s;         // size
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
    struct __align_cfg ca[DIMENSION_COUNT];
} align_mode_t;

#define ALIGN_MODE(hm, ho, vm, vo) \
    (align_mode_t){ \
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

void form_split(form_t * pf, form_t * sf, xy_t * offsets, unsigned x_index, unsigned y_index);
void form_grid(form_t * pf, form_t * af, xy_t * borders, xy_t * gaps, xy_t * grid_size, unsigned x_index, unsigned y_index);

void form_cut(form_t * f, unsigned offset, dimension_t d, form_edge_t edge);
