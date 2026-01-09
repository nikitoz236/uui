#pragma once

#include "font_config.h"

/*
    Итак я смотрю на функцию :

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

    понимаю что большинство проблем здесь математика вычисления положения знакоместа
*/


typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t fxy;       // положение поля в пикселях
    xy_t tlim;      // размеры поля в символах
    xy_t cpos;      // положение курсора в символах

    xy_t cxy;       // положение курсора в пикселях
    xy_t cstep;     // вектор сдвига курсора на 1 символ / строку
} tptr_t;

void text_ptr_init(tptr_t * tptr, const lcd_font_cfg_t * fcfg, xy_t pos_px, xy_t limit_chars);

static inline xy_t text_ptr_current_pos(tptr_t * tptr)
{
    return tptr->cpos;
}

unsigned text_ptr_next_char(tptr_t * tptr);
unsigned text_ptr_prev_char(tptr_t * tptr);
unsigned text_ptr_next_str(tptr_t * tptr);
unsigned text_ptr_set_char_pos(tptr_t * tptr, xy_t pos);
unsigned text_ptr_remain_str(tptr_t * tptr);

// выделяет отдельное поле field внутри tptr для всяких составных лейблов со своими относительными координатами
// все свойства копируются. поле начинается с текущих координат tptr, размер урезается либо до оставшегося справа снизу
// либо если size_chars не нулевой и меньше то до него
tptr_t text_ptr_export(tptr_t * tptr, xy_t size_chars);
