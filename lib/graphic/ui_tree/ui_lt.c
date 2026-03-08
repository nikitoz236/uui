#include "ui_lt.h"


ui_element_t * ui_add(ui_element_t * owner, form_t f, const ui_lt_node_desc_t * desc)
{
    ui_element_t * el = (ui_element_t *)lt_add(&owner->lt, desc);
    el->flags = (typeof(el->flags)){0};
    el->f = f;
    return el;
}


void ui_create(void * ptr, unsigned size, form_t f, ui_lt_node_desc_t * desc)
{
    lt_init(ptr, size);
    ui_element_t * el = ui_add(0, f, desc);

    ui_element_draw(el);
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
