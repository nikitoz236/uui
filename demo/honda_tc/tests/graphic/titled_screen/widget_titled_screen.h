#include "ui_tree.h"
#include "color_scheme_type.h"

extern widget_desc_t __widget_titled_screen;

typedef struct {
    char * title;
    color_scheme_t cs;
    ui_node_desc_t * screen;
} __widget_titled_screen_cfg_t;
