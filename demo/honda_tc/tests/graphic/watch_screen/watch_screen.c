#include <stdio.h>
#include "emu_tc.h"

#include "rtc.h"
#include "date_time.h"

#include "widget_watch_screen.h"

ui_node_desc_t ui = { .widget = &__widget_watch_screen };

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
