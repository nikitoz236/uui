#pragma once
#include "lcd_text.h"
#include "color_type.h"
#include "color_scheme_type.h"

void lcd_text_color_print(const char * c, xy_t * pos, const lcd_text_cfg_t * cfg, const color_scheme_t * cs, unsigned tx, unsigned ty, unsigned len);
