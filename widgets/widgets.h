#pragma once
#include "forms.h"

/*
    система позволяет создавать произвольное дерево графических объектов
    каждый объект является виджетом - набором функций реализующем функционал и описывается структурой widget_t
    дерево виджетов описывается отдельной константной структурой ui_node_t, которая по сути описывает конфигурацию
    виджета через указатель widget_cfg, конфигурация может содержать любую необходимую виджету информацию, в том числе
    описание дочерних виджетов, а то и целых отдельных деревьев

    само построение происходит внутри статически выделенной области памяти в виде стека сверху вниз
    каждая запись описывает виджет через структуру ui_ctx_t, которая хранит геометрию виджета и произвольный контекст

    также запись хранит размер всех дочерних записей для возможности удаления ветви дерева, индекс записи соответствует
    порядку описания дочерних виджетов в конфигурации widget_cfg, для того чтобы после удаления понимать порядок записей в стеке

    все есть виджет, начиная от всего экрана, все лайоуты массивы итд являющтся виджетами и сами определяют как делть и заполнять
    геометрию дочерними виджетами или элементами графики

    для связи интерфейса с бизнес логикой в описании конфигурации виджета можно использовать указатели на обьекты бизнес логики
    а схемы размещения виджетов могут используя указатель на массив данных подсовывать в контекст располагаемого виджета указатель на
    индексные данные для данного виджета

    логика работы:
        сначала по дереву обходятся все виджеты через функцию calc, которая рассчитывает необходимые размеры геометрии
        далее уже вызывается функция draw, которая, зная размеры расчитывает положения и рисует виджеты
*/

typedef struct {            // структура описания динамического элемента в дереве виджетов
    form_t f;               // геометрия виджета
    uint8_t idx;            // индекс данного виджета в описании родительского виджета
    uint8_t child_offset;   // смещение в стеке до первого дочернего виджета
    uint16_t size;          // размер контекста данного виджета в стеке включая всю дочернюю ветвь
    uint8_t ctx[];          // произвольный контекст данного виджета включая все дочерние виджеты
} ui_ctx_t;

typedef struct {            // структура виджета для использования его в описании дерева виджетов

    // функция расчета размера виджета и размера контекста
    // рассчитывает все вложенные виджеты и возвращает размер контекста всей ветви
    // void * cfg           // указатель на конфигурацию виджета
    // ui_ctx_t * node_ctx  // указатель на место в динамическом стеке для размещения контекста
    unsigned (*calc)(void * cfg, ui_ctx_t * node_ctx);

    // функция отрисовки виджета
    // void * cfg           // указатель на конфигурацию виджета
    // void * icfg          // указатель на параметры данного экземпляра виджета
                            // используется для построения массивов одного виджета, например клавиатур
    // ui_ctx_t * node_ctx  // указатель на контекст виджета
    void (*draw)(void * cfg, ui_ctx_t * node_ctx);

    unsigned (*process)(void * widget_cfg, ui_ctx_t * node_ctx, unsigned event);
    uint8_t ctx_len;        // размер контекста только данного виджета
    uint8_t stretched;      // возможно ли менять данному виджету size перед вызовом draw
} widget_t;

typedef struct {            // элемент описания дерева виджетов
    widget_t * widget;      // указатель на виджет
    void * widget_cfg;      // конфигурация виджета (может включать описание дочерних виджетов)
} ui_node_t;

// считает размер прямоугольника виджета и размер контекста, заполняет индекс
static inline unsigned calc_node(const ui_node_t * node, ui_ctx_t * node_ctx, unsigned idx) {
    node_ctx->idx = idx;
    unsigned node_ctx_size = node->widget->calc(node->widget_cfg, node_ctx);
    // поле child_offset каждый виджет должен заполнить сам
    node_ctx->size = node_ctx_size;
    return node_ctx_size;
}

// рисует виджет
static inline void draw_node(const ui_node_t * node, ui_ctx_t * node_ctx) {
    node->widget->draw(node->widget_cfg, node_ctx);
}

static inline ui_ctx_t * next_child(ui_ctx_t * node_ctx)
{
    return (ui_ctx_t *)((void *)node_ctx + node_ctx->size);
}

static inline ui_ctx_t * first_child(ui_ctx_t * node_ctx)
{
    return (ui_ctx_t *)(node_ctx->ctx + node_ctx->child_offset);
}
