#include "emu_common.h"

int main() {
    xy_t app_size = { 640, 480 };
    gfx_open(app_size.x, app_size.y, "UC lib gfx emu device");

    // draw ui here

    while (1) {
        char kbd_btn = gfx_routine();

        if (kbd_btn != 0) {
            printf("recieved keyboard button: %c [%d]\r\n", kbd_btn, kbd_btn);

            if (kbd_btn == 'q') {
                break;
            }
        }
        usleep(1000);
    }

    return 0;
}