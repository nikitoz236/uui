#pragma once

#include "text_color_type.h"
#include "font_config.h"
#include "text_pointer.h"

// попытка сделать чтото более универсальное, чтобы можно было сохранять положение курсора между вызовами, а также через нее выразить все остальные функции печати.
// но тут есть пролема с выравниванием по правому краю и известной длиной, надо либо передавать длину при вызове text_ptr_set_char_pos

void text_print(tptr_t * tptr, const char * str, text_color_t color, unsigned len);
