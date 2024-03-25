#include "ui_tree.h"

extern widget_desc_t __widget_screen_switch;

typedef struct {
    unsigned screens_num;
    ui_node_desc_t * screens_list;
    unsigned * selector_ptr;
} __widget_screen_switch_cfg_t;
