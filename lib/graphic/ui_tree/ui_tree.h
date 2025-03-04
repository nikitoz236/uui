#pragma once
#include <stdint.h>
#include "forms.h"

/*
    модуль организации графического интерфейса
    -   выделяет в отдельном буффере стек элементов интерфейса, каждый элемент является виджетом
    -   у элемента есть заголовок и контекст
    -   структура интерфейса описывается деревом структур ui_node_desc_t

    -   деление на виджеты - атомарность и использование отдельно

    детали реализации
    -   для экономии памяти вместо указателей хранятся смещения, используются функции приобразующие одно в другое

    -   новые виджеты всегда добавляются в конец стека
        логично что дочерние лежат после родителя, дальше следующий элемент на уровне родителя.
        это никак не регулируется, но логично вытекает на стадии добавления элементов
        блягодаря этому удаление дочерних элементов происходит через сдвиг всех следующих элементов на размер удаляемого блока

    обязательства
    -   корневой элемент всегда активен
    -   calc коневого элемента дергается при инициализации, когда уже активен, до задания размера его формы

    -   process_event дергается у всех дочерних элементов рекурсивно, если вернулся 1, то родительский элемент игнорироует событие
        таким образом обеспечивается проваливание в меню

    -   update всегда дергается рекурсивно у всех виджетов через ui_tree_process
        желательно сохранять критерии изменения в контексте виджета, чтобы не заниматься лишней перерисовкой

    -   draw должен отрисовать все по контексту ничего не вычисляя

    -   draw сам выполняет заливку, не должен расчитывать на пустоту своей формы

    обработка виджета
    -   сначала вызывается calc, чтобы узнать минимально необходимые размеры виджета,
        это все виджеты делают самостоятельно после добавления дочерних виджетов, вызывают их calc
        снизу вверх, проводя вычисления, результаты заносятся в f.s, в этот момент инициализируется контекст всех дочерних виджетов

    -   далее зная размер экрана/окна происходит вызов extend, чтобы выполнить выравнивание и распределить элементы,
        каждый виджет также делает это самостоятельно вызывая дочерние виджеты, на этом этапе в контексте хранятся все координаты и размеры
        каждого элемента

    -   вызывается draw, чтобы нарисовать виджет целиком

    -   далее вызывается update, а также если есть событие, элемент активен, и дочерние элементы на него не отреагировали, то вызывается process_event


    ??  нужен ли select ?? кто устанавливает active ??
*/

typedef struct __ui_node_desc ui_node_desc_t;

typedef struct {
    form_t f;                               // форма элемента
    const ui_node_desc_t * ui_node;         // описание элемента
    uint16_t owner;                         // смещение родителя
    uint16_t child;                         // смещение первого дочернего элемента
    uint16_t next;                          // смещение следующего элемента на данном уровне
    uint8_t idx;                            // индекс элемента на данном уровне, позволяет генерировать однотипные списки
    uint8_t active : 1;                     // либо управляет выделением элемента, либо обработкой событий
    uint8_t ctx[];                          // произвольный контекст элемента
} ui_element_t;

typedef struct {
    // расчет минимальных размеров элемента, результат в el->f.s, здесь можно инициализировать контекст, не использует HW
    void (*calc)(ui_element_t * el);

    // пока не используется, но смысл чтобы пересчитывать контекст, после выделения места, не использует HW
    void (*extend)(ui_element_t * el);

    // отрисовка всего элемента
    void (*draw)(ui_element_t * el);

    // обновление элемента - чтобы шли часы, менялись значения итд
    void (*update)(ui_element_t * el);

    // смена статуса выделения элемента, оптимизировано чтобы не дергать draw
    void (*select)(ui_element_t * el);

    // обработка событий, возвращает 1 если событие обработано, что бы родительский элемент не обрабатывал событие
    unsigned (*process_event)(ui_element_t * el, unsigned event);

    // размер контекста элемента
    uint16_t ctx_size;
} widget_desc_t;

// структура описатель виджета - хранится во флеше
struct __ui_node_desc {
    // указатель на сам виджет ( структуру widget_desc_t )
    const widget_desc_t * widget;

    // указатель на конфигурацию виджета
    const void * cfg;
};


// инициализация всего массива, устанавливает заголовок хранящий ограничители и текущее положение свободного участка
// прописывает корневой виджет
void ui_tree_init(void * ptr, unsigned size, const ui_node_desc_t * ui_node, const xy_t * display_size);

// диагностическая функция позволяющая узнать сколько место в буфере сейчас занимает весь интерфейс
unsigned ui_tree_len(void);

// получение соответствующих элементов относительно данного
ui_element_t * ui_tree_owner(ui_element_t * element);
ui_element_t * ui_tree_child(ui_element_t * element);
ui_element_t * ui_tree_next(ui_element_t * element);
ui_element_t * ui_tree_child_idx(ui_element_t * el, unsigned idx);

// функция создает в стеке дочерний элемент, добавля его в конец всего стека, привязывая к хозяину
// индекс прописывается в заголовок элемента
ui_element_t * ui_tree_add(ui_element_t * owner, const ui_node_desc_t * ui_node, unsigned idx);

void ui_tree_delete_childs(ui_element_t * element);


void ui_tree_element_update(ui_element_t * element);
void ui_tree_element_draw(ui_element_t * element);
void ui_tree_element_calc(ui_element_t * element);
unsigned ui_tree_element_select(ui_element_t * element, unsigned select);
void ui_tree_element_extend(ui_element_t * el);

void ui_tree_draw(void);
void ui_tree_update(void);

void ui_tree_process(unsigned event);

    // найти хозяина, и следующего, прописать в хозяина след ребенка и схлопнуть весь стек влево на размер удаляемого элемента
    // ui_new_element(owner_offset, ctx_size, ui_node)

void ui_tree_debug_print_linear(void);
void ui_tree_debug_print_tree(void);
void ui_tree_debug_el_offset(ui_element_t * element);
