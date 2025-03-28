#include <stdio.h>
#include "console_getc.h"
#include "dlc_poll.h"
#include "dlc_emu.h"
#include "systick.h"

/*
    тест модуля dlc_poll

    - проверяет как работает планировщик пакетов
    - проверяет структуру пакетов
    - возвращает какие то реальные данные
    - проверяет как работает таймаут и вызывается tc_engine_set_status
    - позволяет вклинить запрос данных из других блоков для рисования дампов
*/

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
    printf(
        "DLC test!\n"
        "   r - run engine\n"
        "   s - stop engine\n"
        "   E - request data from ECU unit\n"
        "   A - request data from ABS unit\n"
        "   S - request data from SRS unit\n"
    );
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
        }
        usleep(10000);
        dlc_poll();
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
