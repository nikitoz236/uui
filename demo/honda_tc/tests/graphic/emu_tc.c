#include "emu_tc.h"

#include "emu_graphic.h"
#include "emu_lcd.h"

#include "tc_events.h"

#include "systick.h"
#include "mstimer.h"
#include "rtc.h"

volatile unsigned uptime_ms = 0;

void view_process(char key)
{
    ui_tree_update();

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
    } else if (key == 'J') {
        event = EVENT_LONG_BTN_DOWN;
    } else if (key == 'K') {
        event = EVENT_LONG_BTN_UP;
    } else if (key == '\n') {
        event = EVENT_LONG_BTN_OK;
    } else if (key == 'H') {
        event = EVENT_LONG_BTN_LEFT;
    } else if (key == 'L') {
        event = EVENT_LONG_BTN_RIGHT;
    }

    uptime_ms = systick_get_uptime_ms();

    // static mstimer_t timer = { .timeout = 1000 };

    // if (mstimer_do_period(&timer)) {
    //     printf("uptime: %d\n", uptime_ms);
    // }

    if (event) {
        ui_tree_process_event(event);
    }
}

void emu_ui_node(ui_node_desc_t * node)
{
    init_systick();
    init_rtc();

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    form_t lcd_form = {};

    emu_lcd_init(&lcd_cfg, &lcd_form);
    emu_graphic_init(lcd_form.s.w, lcd_form.s.h);
    emu_lcd_clear();

    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, node, &lcd_cfg.size);

    ui_tree_draw();

    emu_graphic_loop(view_process);
}
