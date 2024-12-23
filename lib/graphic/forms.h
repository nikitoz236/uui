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
