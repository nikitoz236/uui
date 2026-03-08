#include "ui_lt.h"


void ui_create(void * ptr, unsigned size, form_t f, ui_lt_node_desc_t * desc)
{
    lt_init(ptr, size);
    ui_element_t * el = lt_item_from_offset(0);
    el->f = f;
    el->lt.desc = desc;

    ui_element_draw(el);
}

ui_element_t * ui_add(ui_element_t * owner, const ui_lt_node_desc_t * desc)
{
    ui_element_t * el = lt_add(&owner->lt, desc);
}

void ui_element_draw(ui_element_t * el)
{
    if (ui_node_desc(el)->widget->draw) {
        ui_node_desc(el)->widget->draw(el);
    }
}

void ui_select(ui_element_t * el, unsigned state)
{
    el->flags.focused = state;
    ui_node_desc(el)->widget->select(el);
}
