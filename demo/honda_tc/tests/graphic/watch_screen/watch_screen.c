#include <stdio.h>
#include "emu_tc.h"

#include "rtc.h"
#include "date_time.h"

#include "widget_watch_screen.h"
#include "widget_screen_switch.h"

#include "widget_selectable_list.h"
#include "widget_time_settings.h"


extern font_t font_5x7;

int timezone_s = 5 * 60 * 60;

int time_zone_get(void)
{
    return timezone_s;
}

void time_zone_set(int tz)
{
    timezone_s = tz;
}

ui_node_desc_t ui = {
    .widget = &widget_selectable_list,
    .cfg = &(widget_selectable_list_cfg_t) {
        .num = 3,
        .margin = { .x = 4, .y = 4 },
        .ui_node = (ui_node_desc_t[]) {
            // {
            //     .widget = &__widget_time_settings
            // },
            // {
            //     .widget = &__widget_date_settings
            // },
            {
                .widget = &widget_settings_title
            },
        }
    }
};

int main()
{
    printf("test watch screen\r\n");

    date_t d = { .d = 26, .m = 12, .y = 2024 };
    time_t t = { .h = 8, .m = 25, .s = 0 };
    unsigned days = days_from_date(&d);
    // printf("days %d\r\n", days);
    unsigned time_s = days_to_s(days) + time_to_s(&t);
    rtc_set_time_s(time_s);

    emu_ui_node(&ui);
    return 0;
}
