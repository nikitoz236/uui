#include <stdio.h>
#include "ui_tree.h"

#include "emu_lcd.h"
#include "widget_metric_list.h"
#include "widget_metric_list_item.h"
#include "metrics_view.h"

void view_process(char key)
{
    unsigned event = 0;
    if (key == 'k') {
        event = 1;
    } else if (key == 'j') {
        event = 2;
    }

    ui_tree_process_event(event);
}

int main()
{
    printf("test honda widget metrics list\r\n");

    __widget_emu_lcd_cfg_t emu_lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    emu_lcd_init(&emu_lcd_cfg);

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
    ui_tree_init(ui_ctx, 1024, &ui, &emu_lcd_cfg.size);

    ui_tree_draw();
    emu_lcd_loop(view_process);

    return 0;
}
