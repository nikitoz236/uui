#include "ui_test_env.h"

#define UI_MEM_SIZE     1024

static uint8_t ui_memory[UI_MEM_SIZE];

static void view_process(char key)
{
    ui_event_t event = 0;
    if (key == 'j') {
        event = EVENT_BTN_DOWN;
    } else if (key == 'k') {
        event = EVENT_BTN_UP;
    } else if (key == ' ') {
        event = EVENT_BTN_OK;
    } else if (key == 'h') {
        event = EVENT_BTN_LEFT;
    }

    if (event) {
        ui_process_event(event);
    }
}

void ui_test_run(const ui_lt_node_desc_t * ui)
{
    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    emu_graphic_init(&lcd_cfg);

    ui_create(ui_memory, UI_MEM_SIZE, (form_t){ .s = lcd_cfg.size }, ui);

    emu_graphic_loop(view_process);
}
