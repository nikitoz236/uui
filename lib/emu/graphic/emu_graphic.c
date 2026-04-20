#include "emu_graphic.h"
#include <unistd.h>

void emu_graphic_loop(void (*process)(char key))
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
