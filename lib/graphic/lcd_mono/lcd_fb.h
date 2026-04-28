#pragma once
#include <stdint.h>
#include "flex_wrap.h"
#include "coord_type.h"
#include "color_type.h"

/*
    фреймбуффер для монохромного page-oriented дисплея
    (8 пикс / байт в вертикальный столбик, страницы сверху вниз)

    один экземпляр на прошивку. имя __lcd_fb_desc фиксировано -
    библиотека ссылается через extern.

    LCD_FB_CREATE(w, h) - в одном .c проекта, обычно рядом с main
*/

typedef uint16_t lcd_fb_size_t;

typedef struct {
    lcd_fb_size_t rstart;
    lcd_fb_size_t rend;
    uint8_t fb[];
} fb_t;

typedef struct {
    fb_t * fb_ctx;
    lcd_fb_size_t fb_len;
    coord_t w;
    coord_t h;
    uint8_t rows;
} fb_desc_t;

#define LCD_FB_PAGES(h) (((h) + 7) / 8)
#define LCD_FB_SIZE(w, h) ((w) * LCD_FB_PAGES(h))

#define LCD_FB_CREATE(lcd_w, lcd_h) \
    const fb_desc_t __lcd_fb_desc = { \
        .fb_ctx = FLEX_WRAP(fb_t, LCD_FB_SIZE(lcd_w, lcd_h)), \
        .fb_len = LCD_FB_SIZE(lcd_w, lcd_h), \
        .w = lcd_w, \
        .h = lcd_h, \
        .rows = LCD_FB_PAGES(lcd_h) \
    }

void lcd_clear(void);
void lcd_refresh(void);

/*
    рисует битовую картинку в fb

    data — column-major, ceil(h / 8) байт на столбец
        в байте бит 0 = верхний пиксель столбца
        последний байт может быть неполным — лишние биты в нём игнорируются
        соседние столбцы не делят байт, граница байта между столбиками всегда

    inverted — 0: бит 1 в data → пиксель fb 1, бит 0 → 0
               1: бит 1 в data → пиксель fb 0, бит 0 → 1

    scale — масштаб, 0 трактуется как 1
*/
void lcd_image_bitmask(unsigned x, unsigned y, unsigned w, unsigned h, const uint8_t * data, unsigned scale, unsigned inverted);
