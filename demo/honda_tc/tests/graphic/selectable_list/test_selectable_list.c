#include <stdio.h>
#include "emu_tc.h"

#include "lcd_text.h"
#include "lcd_text_color.h"
#include "align_forms.h"
#include "str_utils.h"
#include "draw_color.h"

#include "widget_selectable_list.h"
#include "widget_text.h"

extern font_t font_5x7;

lcd_font_cfg_t fcgf = {
    .font = &font_5x7,
    .gaps = { .x = 2, .y = 2 },
    .scale = 2
};

color_scheme_t cs_sel = {
    .fg = COLOR_TEXT,
    .bg = COLOR_BG_SELECTED
};

color_scheme_t cs_unsel = {
    .fg = COLOR_TEXT,
    .bg = COLOR_BG_UNSELECTED
};

ui_node_desc_t ui = {
    .widget = &__widget_selectable_list,
    .cfg = &(__widget_selectable_list_cfg_t) {
        .num = 11,
        .different_nodes = 1,
        .ui_node = (ui_node_desc_t[]) {
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "Hello, world!",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 2, .y = 2 },
                        .scale = 2
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "hey bitch!",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 4, .y = 4 },
                        .scale = 4
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "fuck you!",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 4, .y = 4 },
                        .scale = 4
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "i want chocolate1!",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 2, .y = 2 },
                        .scale = 2
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "i want chocolate!2",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 2, .y = 2 },
                        .scale = 2
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "i want chocolate!3",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 2, .y = 2 },
                        .scale = 2
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "i want chocolate!4",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 2, .y = 2 },
                        .scale = 2
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "snow good!",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 4, .y = 4 },
                        .scale = 4
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "i want chocolate!5",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 2, .y = 2 },
                        .scale = 2
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "i want chocolate!6",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 2, .y = 2 },
                        .scale = 2
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .text = "i like tits!",
                    .fcgf = &(lcd_font_cfg_t) {
                        .font = &font_5x7,
                        .gaps = { .x = 3, .y = 3 },
                        .scale = 3
                    },
                    .cs_selected = &cs_sel,
                    .cs_unselected = &cs_unsel,
                    .padding = { .x = 2, .y = 2 }
                }
            },

        }
    }
};

int main()
{
    printf("test widget_selectable_list\r\n");
    emu_ui_node(&ui);
    return 0;
}
