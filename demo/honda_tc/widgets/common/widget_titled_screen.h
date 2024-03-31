#include "ui_tree.h"
#include "color_scheme_type.h"

extern widget_desc_t __widget_titled_screen;

typedef struct {
    lcd_color_t color_text;
    lcd_color_t color_bg_unselected;
    lcd_color_t color_bg_selected;
    uint8_t screen_num;
    const char ** titles;
    ui_node_desc_t * screen_list;
} __widget_titled_screen_cfg_t;
