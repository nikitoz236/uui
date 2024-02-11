#pragma once
#include "ui_tree.h"

extern widget_desc_t __widget_emu_lcd;

typedef struct __attribute__((packed)) {
    xy_t size;              // разрешение эмулируемого экрана
    int scale;              // размер пикселя эмулируемого экрана в пикселях монитора
    int px_gap;             // зазор между эмулируемыми пикселями в пикселях монитора
    int border;             // размер рамки в пикселях монитора
    int bg_color;           // цвет фона экрана (рамки и зазоров)
} __widget_emu_lcd_cfg_t;
