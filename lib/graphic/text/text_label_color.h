#pragma once
#include "color_scheme_type.h"
#include "text_field.h"
#include "val_text.h"
#include <stddef.h>





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
    union {
        uint16_t ofs;       // val offset in ctx
        uint16_t step;      // step for text_list
    };
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
        // функция которая в контексте данного уровня списка обновит значение
        // тут дикий бардак с индексами, на первом уровне списка индекс - это индекс виджета, функция label_color_list всем подсовывет свой idx из аргумента
        // а вот дальше, для сублэйблов, это вообще не индекс а значение которое надо преобразовать, лежащее в контексте первого уровня
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




typedef struct {
    lcd_color_t color;
    label_t l;
} label_color_t;

void label_color(const tf_ctx_t * tf, const label_color_t * label, lcd_color_t bg_color, unsigned idx, void * ctx, void * prev_ctx);
void label_color_list(const tf_ctx_t * tf, const label_list_t * list, lcd_color_t bg_color, unsigned idx, void * ctx, void * prev_ctx);


void lp(const tf_ctx_t * tf, const label_t * l, color_scheme_t * cs, unsigned idx, void * ctx, void * prev_ctx);
void lcd_label_list(const tf_ctx_t * tf, const label_list_t * list, const color_scheme_t * cs, unsigned idx, void * ctx, void * prev_ctx);
