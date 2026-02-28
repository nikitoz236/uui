#pragma once
#include <unistd.h>
#include "xy_type.h"

void emu_graphic_init_xy(xy_t size);
char emu_routine(void);

static inline void emu_graphic_loop(void (*process)(char key))
{
    while (1) {
        char key = emu_routine();
        if (key == 'q') {
            break;
        }
        if (process) {
            process(key);
        }

        usleep(100000);
    }
}
