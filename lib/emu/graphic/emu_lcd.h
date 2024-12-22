#pragma once
#include "forms.h"
#include "api_lcd_color.h"

typedef struct {
    xy_t size;                  // разрешение эмулируемого экрана в пискелях самого экрана
    unsigned scale;             // размер пикселя эмулируемого экрана в пикселях монитора
    unsigned px_gap;            // зазор между эмулируемыми пикселями в пикселях монитора
    unsigned border;            // размер рамки в пикселях монитора
    unsigned bg_color;          // цвет фона экрана (рамки и зазоров)
} emu_lcd_cfg_t;

void emu_lcd_init(emu_lcd_cfg_t * cfg, form_t * f);
void emu_lcd_clear(void);
