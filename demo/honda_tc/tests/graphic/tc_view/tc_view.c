#include <stdio.h>
#include "emu_graphic.h"
#include "event_list.h"
#include "ui_tree.h"

#include "view.h"

void emu_view_process(char key)
{
    unsigned event = 0;
    if (key == 'j') {
        event = EVENT_BTN_DOWN;
    } else if (key == 'k') {
        event = EVENT_BTN_UP;
    } else if (key == ' ') {
        event = EVENT_BTN_OK;
    } else if (key == 'h') {
        event = EVENT_BTN_LEFT;
    } else if (key == 'l') {
        event = EVENT_BTN_RIGHT;
    }

    view_process(event);
}

int main()
{
    printf("test widget titled screen\r\n");


    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    emu_graphic_init(&lcd_cfg);

    view_screen_on();

    emu_graphic_loop(emu_view_process);

    return 0;
}
