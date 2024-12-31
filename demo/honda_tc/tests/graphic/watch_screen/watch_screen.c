#include <stdio.h>
#include "emu_tc.h"

#include "rtc.h"
#include "date_time.h"

#include "widget_watch_screen.h"
#include "widget_screen_switch.h"
#include "widget_test_rect_with_text.h"

#include "widget_time_settings.h"

unsigned screen_selector = 0;

// ui_node_desc_t ui = {
//     .widget = &__widget_screen_switch,
//     .cfg = &(__widget_screen_switch_cfg_t){
//         .selector_ptr = &screen_selector,
//         .screens_num = 2,
//         .screens_list = (ui_node_desc_t[]){
//             {
//                 .widget = &__widget_watch_screen,
//             },
//             {
//                 .widget = &__widget_test_rect_with_text,
//                 .cfg = &(__widget_test_rect_with_text_cfg_t){
//                     .text = "settings",
//                     .cs = {
//                         .bg = 0x708090,
//                         .fg = 0xfffa75
//                     }
//                 }
//             },
//         }
//     }
// };

ui_node_desc_t ui = {
    .widget = &__widget_time_settings,
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
