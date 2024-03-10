#include <stdio.h>
#include "console_getc.h"
#include "dlc_poll.h"
#include "dlc_emu.h"
#include "systick.h"

void metric_ecu_data_ready(unsigned addr, const uint8_t * data, unsigned len)
{
    printf("DLC metric_ecu_data_ready! addr %d len %d\n", addr, len);
}

void tc_engine_set_status(unsigned state)
{
    printf("tc_engine_set_status %d\n", state);
}

unsigned uptime_ms = 0;

int main() {
    printf("DLC test!\n");
    init_systick();

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
        }
        usleep(10000);
        dlc_poll();
    }

    return 0;
}
