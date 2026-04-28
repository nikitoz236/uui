#pragma once
#include <stdint.h>

typedef struct {
    unsigned scale;
    unsigned px_gap;
    unsigned border;
    unsigned bg_color;
    unsigned on_color;
    unsigned off_color;
    unsigned msb_top;
    unsigned flip_180;
    unsigned px_byte_debug;
    unsigned px_byte_debug_color;
} emu_fb_lcd_cfg_t;

void emu_lcd_mono_init(const emu_fb_lcd_cfg_t * cfg);
