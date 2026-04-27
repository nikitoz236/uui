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
