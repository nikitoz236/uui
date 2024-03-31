#include <stdio.h>
#include "ui_tree.h"
#include "emu_lcd.h"
#include "tc_events.h"
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

    __widget_emu_lcd_cfg_t emu_lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    emu_lcd_init(&emu_lcd_cfg);
    view_screen_on();

    emu_lcd_loop(emu_view_process);

    return 0;
}
