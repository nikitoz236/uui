#pragma once
#include "xy_type.h"

typedef struct {
    xy_t size;                  // разрешение эмулируемого экрана в пискелях самого экрана
    unsigned scale;             // размер пикселя эмулируемого экрана в пикселях монитора
    unsigned px_gap;            // зазор между эмулируемыми пикселями в пикселях монитора
    unsigned border;            // размер рамки в пикселях монитора
    unsigned bg_color;          // цвет фона экрана (рамки и зазоров)
} emu_lcd_cfg_t;

void emu_init(emu_lcd_cfg_t * cfg);
void emu_lcd_clear(void);

char emu_routine(void);
void emu_lcd_px(int x, int y, int color);
void emu_lcd_byte_gap_h(unsigned y, unsigned color);
