#pragma once

#include "text_field.h"

typedef struct {
    tf_t tf;
    xy_t cpos;      // положение курсора в символах
    xy_t cxy;       // положение курсора в пикселях
    xy_t cstep;     // вектор сдвига курсора на 1 символ / строку
} tptr_t;

tptr_t text_ptr_create(tf_t tf);

static inline xy_t text_ptr_current_pos(tptr_t * tptr)
{
    return tptr->cpos;
}

unsigned text_ptr_next_char(tptr_t * tptr);
unsigned text_ptr_prev_char(tptr_t * tptr);
unsigned text_ptr_next_str(tptr_t * tptr);
unsigned text_ptr_set_char_pos(tptr_t * tptr, xy_t pos);
unsigned text_ptr_remain_str(tptr_t * tptr);

// выделяет отдельное поле field внутри tptr для всяких составных лейблов со своими относительными координатами
// все свойства копируются. поле начинается с текущих координат tptr, размер урезается либо до оставшегося справа снизу
// либо если size_chars не нулевой и меньше то до него
tptr_t text_ptr_export(tptr_t * tptr, xy_t size_chars);
