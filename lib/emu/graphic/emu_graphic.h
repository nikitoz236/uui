#pragma once
#include <unistd.h>
#include "emu_lcd.h"

char emu_routine(void);

void emu_graphic_init(emu_lcd_cfg_t * cfg);

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
