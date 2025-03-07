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

static const tf_cfg_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .x = 2, .y = 2 },
        .scale = 2
    },
    .limit_char = { .y = 1 },
    .padding = { .x = 2, .y = 2 },
    .a = ALIGN_LIC,
};

static const color_scheme_t bg_list[] = {
    COLOR_BG_UNSELECTED,
    COLOR_BG_SELECTED,
};

static const label_color_t labels = {
    .color = COLOR_FG_SELECTED,
    .l = {
        .t = LP_T_LIDX,
        .text_list = (const char * []){
            "Hello, world!",
            "hey bitch!",
            "fuck you!",
            "i want chocolate1!",
            "i want chocolate!2",
            "i want chocolate!3",
            "i want chocolate!4",
            "snow good!",
            "i want chocolate!5",
            "i want chocolate!6",
            "i like tits!"
        }
    }
};

ui_node_desc_t ui = {
    .widget = &widget_selectable_list,
    .cfg = &(widget_selectable_list_cfg_t) {
        .num = 11,
        .ui_node = (ui_node_desc_t[]) {
            {
                .widget = &widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .tf_cfg = &tf,
                    .label = &labels,
                    .bg = bg_list,
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
