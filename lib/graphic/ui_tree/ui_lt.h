#pragma once
#include "ltree.h"
#include "forms.h"

/*

    план делать короткие desc

    form и индексы и остальное будет уже внутри конекстов
    нужны ли индексы вообще ?


    нужен флаг drawed чтобы понять влез виджет или нет, чтобы листы могли знать сколько элементов есть и сколько отрисовывать.
    нужен actve чтобы выделять элемент визуально. а еще он же используется для перехвата событий.

    всплывающая модель обработки событий


    в отличии от ui_tree
    1. попапы
    2. многоуровневые индексы родителей
    3. можно удалять детей
    4. конфиг виджета живет в описателе ноды а не в отдельной структре по указателю


*/

typedef struct {
    lt_item_t lt;
    form_t f;                           // форма элемента
    struct {
        uint8_t overridable : 1;
        uint8_t skip        : 1;
        uint8_t focused     : 1;
    } flags;
    uint8_t ctx[];
} ui_element_t;

// typedef lt_item_t ui_element_t;

typedef unsigned ui_event_t;

typedef struct {
    uint16_t ctx_size;
    // unsigned (*calc) (ui_element_t * el);
    void (*draw) (ui_element_t * el);
    void (*select) (ui_element_t * el);
    void (*update) (ui_element_t * el);
    unsigned (*process)(ui_element_t * el, ui_event_t event);
} widget_desc_t;

// описатель ноды — хранится во флеше, передаётся в lt_add как desc
typedef struct {
    const widget_desc_t * widget;
    const void * cfg;
} ui_lt_node_desc_t;

// using form fw
void ui_create(void * ptr, unsigned size, form_t f, ui_lt_node_desc_t * desc);


// using from widget
ui_element_t * ui_add(ui_element_t * owner, form_t f, const ui_lt_node_desc_t * desc);
void ui_element_draw(ui_element_t * el);
void ui_select(ui_element_t * el, unsigned state);

static inline void ui_delete_childs(ui_element_t * el) { lt_delete_childs(&el->lt); };
ui_element_t * ui_child(ui_element_t * el);

// helpers
static inline const ui_lt_node_desc_t * ui_node_desc(ui_element_t * el) { return lt_item_desc(&el->lt); };
#define WIDGET_CTX_NONE (sizeof(ui_element_t) - sizeof(lt_item_t))
#define WIDGET_CTX_SIZE(type) (sizeof(type) + WIDGET_CTX_NONE)
