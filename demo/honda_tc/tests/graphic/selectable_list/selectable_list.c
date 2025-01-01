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

ui_node_desc_t ui = {
    .widget = &__widget_selectable_list,
    .cfg = &(__widget_selectable_list_cfg_t) {
        .num = 11,
        .different_nodes = 0,
        .ui_node = (ui_node_desc_t[]) {
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "Hello, world!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "hey bitch!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "fuck you!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate1!",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!2",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!3",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!4",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "snow good!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!5",
                    .scale = 2
                }
            },            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!6",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i like tits!",
                    .scale = 3
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
