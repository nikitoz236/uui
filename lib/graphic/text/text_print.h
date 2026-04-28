#pragma once

#include "text_color_type.h"
#include "font_config.h"
#include "text_pointer.h"

void lcd_color_tptr_print(tptr_t * tptr, const char * str, text_color_t color, unsigned len);
