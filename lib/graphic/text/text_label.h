#pragma once
#include "val_pack.h"
#include "val_text.h"
#include "text_pointer.h"
#include "lists.h"
#include "color_scheme_type.h"



/*
    итак есть маршруты

    есть виджет маршрута, он отображает параметры както, среди них числа и например время которое рисуется сублэйблами,
    допустим я смог в контексте развернуть структуру в которой лежат данные для отображения

    задача 1:
        отрисовка виджета,

        виджет является элементом списка, потому что этих маршрутов несколько разных.
        кроме того списков несколько. это маршруты и 2 истории поездок и заправок
        итого есть списки и виджет параметризуется

        виджет знает свой индекс, в контексте виджета гдето есть тип списка

        виджет должен при инициализации должен вызвать функцию и прочитать данные для отрисовки, передав ей указатель на аллоцированный контест

        дальше при обновлени вызывается функция которая получает теже данные в структуру на стеке
            выполняется сравнение каждого поля

        а ведь весь виджет это и есть сублэйбл, просто мы в качестве значения передаем ему индекс



    итак мне нужно рисовать и перерисовывать обьекты

    например таблицу константных данных, либо прям меняющиеся данные,
    строка состояния - виджет который имеет поля меняющиеся, часы заряд акума, язык, шифты итд

        где
            координаты пикселей или букв где будет расположена надпись
            количество знакомест или даже лимиты ?

        что
            указатель на контекст в памяти - аргумент отрисовки
            оффсет в контексте констанстный
            длина поля данных


        как
            субсписок лэйблов

            статичный текст по указателю
            текст из функции по значению
            текст из функции по индексу
            текст из указателя по значению
            текст из указателя по индексу

            текст enum в зависимости от индекса виджета
            текст enum в зависимости от значения переменной

            хекс ( сколько байт ? )
            картинка по индексу
            картинка включеная или нет
            составная
            указатель на текст


        LP_S,               // sub label
        LP_V,               // value to str, format from const vt
        LP_VC,              // const value
        LP_V_FIDX,          // value to str, format from function by index
        LP_T,               // text from const pointer
        LP_T_FIDX,          // text from to_str function by index
        LP_T_FV,            // text from to_str function by unsigned val
        LP_T_LIDX,          // text from pointers array list by index
        LP_T_LV,            // text from pointers array list by unsigned val
*/


typedef union {
    void (*u)(void * ctx, unsigned val);
    void (*s)(void * ctx, int val);
    void (*p)(void * ctx, void * val);
} ctx_update_func_t;


enum label_type {
    LABEL_SUB_LIST,             // по указателю список структур label_t, вызывается функция

    LABEL_STATIC_TEXT,
    LABEL_VAL,
    // LABEL_,
};

typedef struct {
    list_t * labels_static;             // labels const or static
    list_t * labels_dynamic;            // labels volatile
    ctx_update_func_t update_func;
} label_sublist_t;


typedef struct {
    enum label_type type;               // тип обработки
    uint8_t color_idx : 4;              // цвет из палитры

    uint8_t vt_ctx : 1;                 // vt из ctx по vt_offset
    // uint8_t val_idx : 1;                // использовать idx как значение поля

    val_rep_t val_rep;                  // представление значения поля по оффсету в контексте, преобразовать в uint8_t : 3

    uint8_t val_offset_in_ctx;          // оффсет значения поля в контексте

    union {
        uint8_t sub_list_ctx_offset;    // для сублиста 
        uint8_t len;
    };

    xy_t pos;

    union {
        const char * text;
        // const char * text;
        label_sublist_t sublist;
        union {
            uint8_t vt_offset;
            val_text_t vt;
        };
    };
} label_t;


/*
    а может здесь не нужен статический лист?

    а еще если мне не нужно значнеие и я использую индекс, то мне нужно преобразовать его в текст
        то я ничем его не заменю всеравно
    по индексу или по значению
    текст ?
        статичный
        из массива указателей
            условный текст
        из функции

    а если значнеие то нужен офсет и кодирование. ну это 3 бита,
*/


typedef struct {
    lcd_color_t * color_palette;
    // list_t * static_labels;
    label_t label;
} widget_labels_t;

void widget_labels_proces(const widget_labels_t * l, tptr_t * tp, void * ctx, void * ctx_prev);
