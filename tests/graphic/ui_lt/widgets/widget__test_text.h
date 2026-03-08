#pragma once
#include "ui_lt.h"

/*
    simple text widget for tests
*/

extern const widget_desc_t widget__test_text;

struct widget_ctx_struct__test_text {
    char * text;
};

// struct widget_ctx_struct__test_text * widget_ctx__test_text(ui_element_t * el) { return el->}
