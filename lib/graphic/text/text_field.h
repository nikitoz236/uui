#pragma once

#include "font_config.h"
#include "val_text.h"
#include "forms_align.h"
#include <stddef.h>

// получается что это один из вариантов универсального блока интерфейса, допустим еще есть картинки или какаято графика. хз, главное что мы располагая их оперируем формами
typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t limit_char;            // если 0 то будет посчитано все доступное пространство
    xy_t padding;               // внутреннее смещение текста от краев
    align_t a;
} tf_cfg_t;

typedef struct {
    const tf_cfg_t * tfcfg;
    xy_t xy;        // вместо margin и padding сразу результат
    xy_t size;      // вместо limit_char
} tf_ctx_t;


/*
    это какаято суперфункция на самомо деле которая сводит размеры доступного места с тем что нужно нарисовать
    limit_char если 0 то доступное место по форме, если нет то форма уменьшается


*/
// возвращаемое значение показывает получилось ли втиснуться в размер
unsigned tf_ctx_calc(tf_ctx_t * ctx, form_t * f, const tf_cfg_t * cfg);





/*

итак значит мне сейчас надо сейчас нахуевертить механизм отрисовки чегонибудь

у тебя есть текстовое поле и к нему массив структур, количество структур.

даже несколько массивов. статический и динамический

для статической надписи тебе надо либо указатель на текст, либо на указатель по индексу

для динамической надписи тебе надо указатель на функцию которая преобразует значение в текст

задача в имеющуюся родительскую форму вписать какойто произвольный конфиг обьектов

    обьект эт прямоугольник, тоесть form, он может по каждой координате:
    занять все доступоное пространство - как двум таким умникам поделить одно пространство ?
    иметь какойто минимум, зависимость от текста который мы хотим в нем расположить

я хочу заебашить чтобы у меня считалась форма сама по конфигу, и если там в лимитах 0 по кординате то она не меняется
    либо можно передавать в размеры родительской формы

    опять таки надо координаты

например часы - там точно 5 символов и тебе надо ебануть это по центру, либо склеить часы из 5 символов с другим полем в котором только секунды чуть меньше размероми все это центрировать

*/

struct label_list;

typedef struct {
    union {
        const char * text;
        const char * (*to_str)(unsigned x);
        const struct label_list * sl;
        void (*vt_by_idx)(val_text_t * vt, unsigned idx);
        val_text_t vt;
        const char ** text_list;
        // const char * (*to_str_val)(void * p, val_rep_t r);
    };
    xy_t xy;
    union {
        uint16_t len;
        uint16_t sofs;      // sub label ctx offset in ctx
    };
    uint16_t ofs;           // val offset in ctx
    val_rep_t rep;          // val representation in ctx
    enum {
        LP_S,               // sub label
        LP_V,               // value to str, format from const vt
        LP_V_FIDX,          // value to str, format from function by index
        LP_T,               // text from const pointer
        LP_T_FIDX,          // text from to_str function by index
        LP_T_FV,            // text from to_str function by unsigned val
        LP_T_LIDX,          // text from pointers array list by index
        LP_T_LV,            // text from pointers array list by unsigned val

        LV = LP_V,
        LVFI = LP_V_FIDX,
        LF = LP_T_FV,
        LS = LP_S,
    } t : 4;
} label_t;

struct label_list {
    union {
        void (*ctx_update)(void * ctx, unsigned idx);
        void (*ctx_update_signed)(void * ctx, int idx);
    };

    union {
        const label_t * list;
        const void * wrap_list;     // может быть какойто другой тип. на усмотрение реализации печатающей функции.
    };
    uint8_t count;
};

typedef struct label_list label_list_t;
