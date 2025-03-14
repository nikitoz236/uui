#include <stdio.h>
#include "emu_tc.h"
#include "widget_dash.h"
#include "dlc_emu.h"

#include "date_time.h"
#include "rtc.h"
#include "storage.h"

ui_node_desc_t ui = {
    .widget = &widget_dash,
};

int timezone_s = 5 * 60 * 60;

int time_zone_get(void)
{
    return timezone_s;
}

void time_zone_set(int tz)
{
    timezone_s = tz;
}

int main()
{
    printf("test dash screen\r\n");


    init_rtc();
    rtc_set_time_s(date_time_to_s(&(date_t){ .y = 2025, .m = MONTH_MAR, .d = 19 }, &(time_t){ .h = 11, .m = 43, .s = 12 }));

    emu_storage_load();

    storage_init();
    storage_print_info();

    route_load();

    route_trip_start(1);
    emu_engine_ctrl(1);

    emu_ui_node(&ui);


    emu_engine_ctrl(1);
    return 0;
}
