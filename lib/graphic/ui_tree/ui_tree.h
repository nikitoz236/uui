#pragma once
#include <stdint.h>
#include "forms.h"

typedef struct __ui_node_desc ui_node_desc_t;

typedef struct {
    form_t f;
    const ui_node_desc_t * ui_node;
    uint16_t owner;
    uint16_t child;
    uint16_t next;
    uint8_t idx;
    uint8_t active : 1;
    uint8_t ctx[];
} ui_element_t;

typedef struct {
    void (*calc)(ui_element_t * el);
    void (*draw)(ui_element_t * el);
    void (*update)(ui_element_t * el);
    void (*select)(ui_element_t * el, unsigned selected);
    unsigned (*process_event)(ui_element_t * el, unsigned event);
    uint16_t ctx_size;
} widget_desc_t;

struct __ui_node_desc {
    const widget_desc_t * widget;
    const void * cfg;
};

void ui_tree_init(void * ptr, unsigned size, const ui_node_desc_t * ui_node, const xy_t * display_size);
unsigned ui_tree_len(void);

ui_element_t * ui_tree_owner(ui_element_t * element);
ui_element_t * ui_tree_child(ui_element_t * element);
ui_element_t * ui_tree_next(ui_element_t * element);

void * ui_tree_ctx(ui_element_t * element);

// функция создает в стеке дочерний элемент, добавля его в конец всего стека, привязывая к хозяину
ui_element_t * ui_tree_add(ui_element_t * owner, const ui_node_desc_t * ui_node, unsigned idx);

void ui_tree_delete_childs(ui_element_t * element);


void ui_tree_element_update(ui_element_t * element);
void ui_tree_element_draw(ui_element_t * element);
void ui_tree_element_calc(ui_element_t * element);
void ui_tree_element_select(ui_element_t * element, unsigned select);
unsigned ui_tree_element_process(ui_element_t * element, unsigned event);


void ui_tree_draw(void);
void ui_tree_update(void);
void ui_tree_process_event(unsigned event);

    // найти хозяина, и следующего, прописать в хозяина след ребенка и схлопнуть весь стек влево на размер удаляемого элемента
    // ui_new_element(owner_offset, ctx_size, ui_node)

void ui_tree_debug_print_linear(void);
void ui_tree_debug_print_tree(void);
void print_el_offset(ui_element_t * element);
