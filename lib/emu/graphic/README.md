# emu/graphic

Эмуляция графического дисплея на Linux. Цель — отлаживать прошивочный
код рисования без МК. Два бэкенда (X11-окно или AI bitmap), две формы
дисплея (цветной / моно через page-oriented framebuffer).

## Файлы

- `emu_graphic.h/.c` — публичный API: `emu_init`, `emu_graphic_loop`
- `emu_lcd.h` — общий заголовок: `emu_lcd_cfg_t`, `emu_init`,
  `emu_lcd_clear`, `emu_lcd_px`, `emu_routine`
- `emu_lcd.c` — реализация общего API на X11 (через `gfx.c`).
  Рисует масштабированные квадраты пикселей с зазором и рамкой
- `emu_lcd_bitmap.c` — реализация общего API в AI bitmap режиме.
  Хранит символьный fb 1-в-1, по `emu_routine` дампит кадр в
  `ai_frames/NNNN.txt` (Base64-индексированная палитра)
- `emu_lcd_color.c` — цветной API `api_lcd_color.h` (`lcd_rect`,
  `lcd_image`) поверх `emu_lcd_px`
- `emu_fb_lcd.h/.c` — мост моно framebuffer ↔ эмулятор:
  `emu_lcd_mono_init` (строит `emu_lcd_cfg_t` из `__lcd_fb_desc`
  и `emu_fb_lcd_cfg_t`, вызывает `emu_init`), `lcd_refresh`
  (читает page-oriented fb попиксельно, зовёт `emu_lcd_px` с
  `on_color`/`off_color` и опциональным flip/lsb_top)
- `gfx.c/.h` — X11 wrapper
- `emu_graphic.mk` — общий Makefile-include с переключателями `LCD_FB`
  и `AI`

## Сборка

`emu_graphic.mk` собирает один из четырёх вариантов:

                AI не задан                 AI=1
    LCD_FB     X11 цветной                  AI цветной
    не задан   gfx.c + emu_lcd.c +          emu_lcd_bitmap.c +
               emu_lcd_color.c              emu_lcd_color.c

    LCD_FB=1   X11 моно                     AI моно
               gfx.c + emu_lcd.c +          emu_lcd_bitmap.c +
               emu_fb_lcd.c + lcd_fb.c      emu_fb_lcd.c + lcd_fb.c

`emu_graphic.c` подключается всегда.

## Цепочки вызовов

### Цветной X11

    test → emu_graphic_init(cfg) → emu_init (emu_lcd.c)
    test → lcd_rect(x,y,w,h,color) (emu_lcd_color.c)
                → emu_lcd_px (emu_lcd.c)
                → gfx_rect (gfx.c) → XFillRectangle
    test → emu_graphic_loop(process)
                → emu_routine (emu_lcd.c) → gfx_routine

### Цветной AI

    test → emu_graphic_init(cfg) → emu_init (emu_lcd_bitmap.c)
                → calloc(fb), mkdir(ai_frames)
    test → lcd_rect (emu_lcd_color.c)
                → emu_lcd_px (emu_lcd_bitmap.c) → fb_put_pixel
    test → emu_graphic_loop
                → emu_routine (emu_lcd_bitmap.c)
                → flush_frame → ai_frames/NNNN.txt
                → 'q' когда fb не менялся

### Моно X11

    test → emu_lcd_mono_init(fb_cfg) (emu_fb_lcd.c)
                → emu_init(emu_lcd_cfg_t из __lcd_fb_desc.w/h)
    test → lcd_rect (lcd_fb.c) → fb (page-oriented, маски страниц)
    test → lcd_refresh (emu_fb_lcd.c)
                → попиксельно: читает байт страницы + битовая маска
                → emu_lcd_px (emu_lcd.c) с on_color/off_color
                → gfx_rect
    test → emu_graphic_loop

### Моно AI

    test → emu_lcd_mono_init → emu_init (emu_lcd_bitmap.c)
    test → lcd_rect (lcd_fb.c) → fb
    test → lcd_refresh → emu_lcd_px (emu_lcd_bitmap.c) → fb_put_pixel
    test → emu_graphic_loop → emu_routine → ai_frames/NNNN.txt

## Compile-time флаги (моно прошивочная часть, `lcd_fb.c`)

- `LCD_TOP_LSB` / `LCD_TOP_MSB` — порядок битов в странице
  (определяется контроллером дисплея)

  Известные контроллеры:
  - PCF8814 (Nokia 1202) — `LCD_TOP_LSB`
- `LCD_FLIP_180` — экран физически перевёрнут на 180°

В эмуляторе аналогичные флипы — runtime поля
`flip_180`/`msb_top` в `emu_fb_lcd_cfg_t`, чтобы один бинарник
эмулятора показывал любую конфигурацию прошивки.
