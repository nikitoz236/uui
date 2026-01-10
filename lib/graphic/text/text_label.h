#pragma once
#include "val_pack.h"
#include "val_text.h"
#include "text_pointer.h"
#include "lists.h"
#include "color_scheme_type.h"



/*
    никак не могу уловить эту тонкую грань. что мне нужно чтобы нарисовать один вложеный виджет?
        поле для этого виджета
        способ обновить данные и понять что были изменения
        способ отрисовать

        кажется нужно начинать снизу.
            для часиков у меня есть поле часиков



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


// я бы хотел сделать draw_list


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
    list_t * labels;
    // label_t l;
    ctx_update_func_t update_func;
} label_sublist_t;


typedef struct {
    enum label_type type;
    uint8_t color_idx : 4;
    // val_rep_t rep : 3;
    // enum {
    //     LABEL_STATIC,
    //     LABEL_VALUE
    // } need_val : 1;
    // uint8_t need_val : 1;
    uint8_t vt_real : 1;
    uint8_t val_idx : 1;


    val_rep_t val_rep;

    uint8_t val_offset_in_ctx;
    uint8_t sub_list_ctx_offset;

    uint8_t vt_offset;
    val_text_t vt;
    uint8_t len;
    xy_t pos;

    union {
        const char * text;
        label_sublist_t sublist;
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
    list_t * static_labels;
    lcd_color_t * color_palette;
    label_t label;
} widget_labels_t;

void widget_labels_proces(const widget_labels_t * l, tptr_t * tp, void * ctx, void * ctx_prev);

// void label_sub_list_update_ctx(void * val_ptr, val_rep_t, void * ctx)
// {

// }

// unsigned check_vaue(label_t * l, void * ctx_prev, void * ctx, unsigned * val_to_draw)
// {

// }

// void labels_init(label_t * ls, label_t * ld, void * ctx, tptr * tp, color_scheme_t * cs);
// void labels_update(label_t * ld, void * ctx_prev, void * ctx, tptr * tp, color_scheme_t * cs);




// void labels_init(label_t * ls, label_t * ld, void * ctx, tptr * tp, color_scheme_t * cs)
// {

// }
