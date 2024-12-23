#include <stdio.h>
#include "emu_graphic.h"
#include "emu_lcd.h"

#include "ui_tree.h"
#include "widget_metric_list.h"
#include "widget_metric_list_item.h"
#include "tc_events.h"
// #include "metrics_view.h"

void view_process(char key)
{
    unsigned event = EVENT_NONE;
    if (key == 'k') {
        event = EVENT_BTN_UP;
    } else if (key == 'j') {
        event = EVENT_BTN_DOWN;
    }

    ui_tree_process_event(event);
}

int main()
{
    printf("test honda widget metrics list\r\n");

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

    ui_node_desc_t ui = {
        .widget = &__widget_metric_list,
        .cfg = &(__widget_metric_list_cfg_t){
            .ui_node = &(ui_node_desc_t){
                .widget = &__widget_metric_list_item,
                .cfg = &(__widget_metric_list_item_cfg_t){
                    .color_text = 0xfffa75,
                    .color_value = 0x66CFAB,
                    .color_raw = 0xff4450,
                    .color_unit = 0x00ff00,
                    .color_bg = 0
                }
            },
            .borders = { .w = 4, .h = 4 },
            .gaps = { .x = 2, .y = 4 },
            .color_bg = 0,
            .num = METRIC_VAR_ID_NUM
        }
    };

    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, &ui, &lcd_cfg.size);

    ui_tree_draw();

    emu_graphic_loop(view_process);

    return 0;
}
