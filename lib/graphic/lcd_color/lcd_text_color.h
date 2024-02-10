#pragma once
#include "lcd_text.h"
#include "color_type.h"
#include "color_scheme_type.h"

void lcd_text_color_print(char * c, xy_t * pos, lcd_text_cfg_t * cfg, color_scheme_t * cs, unsigned tx, unsigned ty, unsigned len);
