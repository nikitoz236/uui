#pragma once
#include <stdint.h>

// используется для работы с цветным экраном размером больше 256 по одной из координат
// является типом из которого строятся прямоугольники в forms

// подумал что хорошо бы ему быть знаковым. тогда он сможет быть отрицательным. а значит может быть офсетами влево.

typedef int16_t coord_t;
