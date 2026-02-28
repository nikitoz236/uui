/*
    Эмуляция LCD для AI-отладки.
    Заменяет emu_lcd.c (+ gfx) одним файлом.

    Реализует api_lcd_color (lcd_rect, lcd_image) и emu_lcd (init, clear)
    напрямую через символьный фреймбуфер с палитрой.
    Сохраняет ASCII-кадры в ai_frames/ на каждый emu_routine (из emu_graphic_loop).

    Формат кадра (.txt):
        palette:A=RRGGBB,B=RRGGBB,...
        <пустая строка>
        pixels:
        <строки пикселей, Base64 индексы в порядке появления>

    Выход: emu_routine возвращает 'q' когда буфер чистый (нечего рисовать).
*/

#include "emu_graphic.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifndef AI_FRAMES_DIR
#define AI_FRAMES_DIR "ai_frames"
#endif

#define MAX_PALETTE 64

static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char * framebuffer;
static emu_lcd_cfg_t * lcd_cfg;
static unsigned fb_w, fb_h;

static unsigned palette[MAX_PALETTE];
static unsigned palette_count;

static unsigned fb_dirty;
static unsigned frame_counter;

static char output_dir[256] = AI_FRAMES_DIR;

/* ------------------------------------------------------------------ */

static inline char color_to_char(unsigned color)
{
    for (unsigned i = 0; i < palette_count; i++) {
        if (palette[i] == color) {
            return b64[i];
        }
    }
    if (palette_count < MAX_PALETTE) {
        unsigned idx = palette_count++;
        palette[idx] = color;
        return b64[idx];
    }
    return b64[0];
}

/* ------------------------------------------------------------------ */
/*  Пиксель в фреймбуфер                                              */
/* ------------------------------------------------------------------ */

static inline void fb_put_pixel(unsigned x, unsigned y, unsigned color)
{
    if (x >= fb_w || y >= fb_h) {
        return;
    }
    framebuffer[y * fb_w + x] = color_to_char(color);
}

/* ------------------------------------------------------------------ */
/*  ASCII frame writer (Base64-indexed palette)                        */
/* ------------------------------------------------------------------ */

static void save_txt(const char * path)
{
    FILE * f = fopen(path, "w");
    if (!f) {
        return;
    }

    fprintf(f, "palette:");
    for (unsigned i = 0; i < palette_count; i++) {
        if (i > 0) {
            fputc(',', f);
        }
        fprintf(f, "%c=%06X", b64[i], palette[i]);
    }
    fprintf(f, "\n\npixels:\n");

    for (unsigned y = 0; y < fb_h; y++) {
        fwrite(&framebuffer[y * fb_w], 1, fb_w, f);
        fputc('\n', f);
    }

    fclose(f);
}

static void flush_frame(void)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%04u.txt", output_dir, frame_counter);
    save_txt(path);

    frame_counter++;
    fb_dirty = 0;
}

/* ------------------------------------------------------------------ */

char emu_routine(void)
{
    if (!fb_dirty) {
        return 'q';
    }
    flush_frame();
    return 0;
}

/* ------------------------------------------------------------------ */

void emu_graphic_init(emu_lcd_cfg_t * cfg)
{
    lcd_cfg = cfg;
    fb_w = cfg->size.w;
    fb_h = cfg->size.h;
    framebuffer = (char *)calloc(fb_w * fb_h, 1);
    palette_count = 0;

    mkdir(output_dir, 0755);
    frame_counter = 0;

    emu_lcd_clear();
}

void emu_lcd_clear(void)
{
    char bg = color_to_char(lcd_cfg->bg_color);
    memset(framebuffer, bg, fb_w * fb_h);
    fb_dirty = 1;
}

/* ------------------------------------------------------------------ */

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, lcd_color_t color)
{
    for (unsigned dy = 0; dy < h; dy++) {
        for (unsigned dx = 0; dx < w; dx++) {
            fb_put_pixel(x + dx, y + dy, color);
        }
    }
    fb_dirty = 1;
}

void lcd_image(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scale, lcd_color_t * buf)
{
    if (scale == 0) {
        scale = 1;
    }
    for (unsigned sy = 0; sy < h; sy++) {
        for (unsigned sx = 0; sx < w; sx++) {
            lcd_color_t color = *buf++;
            for (unsigned py = 0; py < scale; py++) {
                for (unsigned px = 0; px < scale; px++) {
                    fb_put_pixel(x + sx * scale + px, y + sy * scale + py, color);
                }
            }
        }
    }
    fb_dirty = 1;
}
