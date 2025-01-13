#include <stdio.h>
#include "console_getc.h"
#include "dlc_poll.h"
#include "dlc_emu.h"
#include "systick.h"
#include "rtc.h"
#include "tc.h"
#include "storage.h"
#include "emu_storage.h"
#include "routes.h"

unsigned uptime_ms = 0;

void view_screen_off(void)
{
    printf("view_screen_off\n");
}

void view_screen_on(void)
{
    printf("view_screen_on\n");
}

void view_process(unsigned int event)
{
    // printf("view_process %d\n", event);
}

int main() {
    printf("DLC TC and routes test!\n");
    init_systick();
    init_rtc();
    emu_storage_load();
    storage_init();
    route_load();
    storage_print_info();

    char key = 0;
    while (key != 'q') {
        uptime_ms = systick_get_uptime_ms();
        key = console_getc();
        if (key) {
            if (key == 'r') {
                printf("DLC EMU engine on\n");
                emu_engine_ctrl(1);
            }
            if (key == 's') {
                printf("DLC EMU engine off\n");
                emu_engine_ctrl(0);
            }
            if (key == 'E') {
                printf("DLC EMU request ECU dump\n");
                dlc_dump_request(HONDA_UNIT_ECU);
            }
            if (key == 'A') {
                printf("DLC EMU request ABS dump\n");
                dlc_dump_request(HONDA_UNIT_ABS);
            }
            if (key == 'S') {
                printf("DLC EMU request SRS dump\n");
                dlc_dump_request(HONDA_UNIT_SRS);
            }
            if (key == 'R') {
                printf("\n                             DIST        FUEL        TIME       SINCE   CONS_DIST   CONS_TIME\n");
                for (route_t rt = 0; rt < ROUTE_TYPE_NUM; rt++) {
                    printf("route %14s: ", route_name(rt));
                    for (route_value_t vt = 0; vt < ROUTE_VALUE_NUM; vt++) {
                        printf("%10d  ", route_get_value(rt, vt));
                    }
                    printf("\n");
                }
            }
            if (key == 'C') {
                printf("        select route for clean route: 1 ... 9 \n");
                key = 0;
                while(key == 0) {
                    key = console_getc();
                }
                if (key >= '0' && key <= '0' + ROUTE_TYPE_NUM) {
                    printf("    clean route %d\n", key - '0');
                    route_reset(key - '0');
                }
            }
            if (key == 'H') {
                printf("\ntrip history   DIST        FUEL        TIME       SINCE   CONS_DIST   CONS_TIME\n");
                for (unsigned i = 0; i < TRIP_HISTORY_RECORDS; i++) {
                    printf("trip %2d: ", i);
                    for (route_value_t vt = 0; vt < ROUTE_VALUE_NUM; vt++) {
                        printf("%10d  ", trip_history_get_value(i, vt));
                    }
                    printf("\n");
                }
            }
        }
        usleep(50000);
        dlc_poll();
        tc_routine(0);
        // printf("rtc sec %d\n", rtc_get_time_s());

        uint8_t dump[16];
        unsigned address = 0;
        honda_unit_t unit = HONDA_UNIT_COUNT;
        unsigned len = dlc_dump_get_new_data(dump, &address, &unit);

        if (len) {
            printf("DLC dump new data ready, addr %d, len %d\n", address, len);
        }
    }

    return 0;
}
