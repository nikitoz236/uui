#include <stdio.h>
#include "emu_tc.h"

#include "rtc.h"
#include "date_time.h"

#include "widget_watch_screen.h"
#include "widget_screen_switch.h"
#include "widget_test_rect_with_text.h"

#include "widget_selectable_list.h"
#include "widget_time_settings.h"
#include "widget_text.h"

uint8_t screen_selector = 0;

extern font_t font_5x7;

ui_node_desc_t ui = {
    .widget = &__widget_screen_switch,
    .cfg = &(__widget_screen_switch_cfg_t){
        .selector_ptr = &screen_selector,
        .screens_num = 2,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &__widget_watch_screen,
            },
            {
                .widget = &__widget_selectable_list,
                .cfg = &(__widget_selectable_list_cfg_t) {
                    .num = 3,
                    .different_nodes = 0,
                    .ui_node = (ui_node_desc_t[]) {
                        {
                            .widget = &__widget_time_settings
                        },
                        {
                            .widget = &__widget_text,
                            .cfg = &(widget_text_cfg_t){
                                .text = "one",
                                .color_bg_selected = COLOR_BG_SELECTED,
                                .color_bg_unselected = COLOR_BG_UNSELECTED,
                                .color_text = COLOR_TEXT,
                                .font = &font_5x7,
                                .scale = 4
                            }
                        },
                        {
                            .widget = &__widget_text,
                            .cfg = &(widget_text_cfg_t){
                                .text = "two",
                                .color_bg_selected = COLOR_BG_SELECTED,
                                .color_bg_unselected = COLOR_BG_UNSELECTED,
                                .color_text = COLOR_TEXT,
                                .font = &font_5x7,
                                .scale = 4
                            }
                        },
                    }
                }
            }
        }
    }
};

int main()
{
    printf("test watch screen\r\n");

    date_t d = { .d = 26, .m = 12, .y = 2024 };
    time_t t = { .h = 8, .m = 25, .s = 0 };
    unsigned time_s = days_to_s(days_from_date(&d)) + time_to_s(&t);
    rtc_set_time_s(time_s);

    emu_ui_node(&ui);
    return 0;
}
