#pragma once
#include "lcd_text.h"
#include "color_type.h"
#include "color_scheme_type.h"

#include "font_config.h"

// gaps не учитывают scale
// DEPRICATED
void lcd_text_color_print(const char * c, xy_t * pos, const lcd_text_cfg_t * cfg, const color_scheme_t * cs, unsigned tx, unsigned ty, unsigned len);

// основная функция, позволяет расположить строку str в координатах pos
// cfg - конфигурация шрифта, при этом если cfg.scale = 0, то он считается равным 1, если cfg.gaps = 0, то они считаются равными 1
//      на cfg.scale домножается font, cfg.gaps в пикселях определяет расстояние между символами, не домножается на cfg.scale
// cs - цветовая схема, fg - цвет текста, bg - цвет фона
// limit_chars - ограничение размер текста в символах, если 0 - то просто до безконечности вправо рисует (ну для коротких текстов можно использовать)
//      можно указать только limit_chars.x, тогда будет печататься одна строка (text_size.y = 1)
// pos_chars - положение текста в текстовом поле в символах, если 0 - то в начало,
//      также может быть отрицательным, для выравнивания слева, но нужен limit_chars. при len != 0 считает считает правый край текста. иначе левый
// len - длина строки, если 0 - то до конца текста или до text_size.x
void lcd_color_text_raw_print(const char * str, const lcd_font_cfg_t * cfg, const color_scheme_t * cs, const xy_t * pos_px, const xy_t * limit_chars, const xy_t * pos_chars, unsigned len);

// появилась идея завернуть все в универсальную функцию, color_scheme_t это тип который передается целиком, на монохромном дисплее будет также, но это не точно
static inline void lcd_text(xy_t xy_px, const char * str, const lcd_font_cfg_t * cfg, color_scheme_t cs, const xy_t * limit_chars, const xy_t * pos_chars, unsigned len)
{
    lcd_color_text_raw_print(str, cfg, &cs, &xy_px, limit_chars, pos_chars, len);
}







#include "text_pointer.h"

// попытка сделать чтото более универсальное, чтобы можно было сохранять положение курсора между вызовами, а также через нее выразить все остальные функции печати.
// но тут есть пролема с выравниванием по правому краю и известной длиной, надо либо передавать длину при вызове text_ptr_set_char_pos

// просто ты хочешь наворотить чтобы у тебя перетиралось поле новым значением, чтобы выравнивание было

void lcd_color_tptr_print(tptr_t * tptr, const char * str, color_scheme_t cs, unsigned len);
