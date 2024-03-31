#include <stdio.h>
#include "ui_tree.h"
#include "emu_lcd.h"

#include "tc_events.h"
#include "widget_test_rect_with_text.h"
#include "widget_titled_screen.h"

#include "widget_screen_switch.h"

#include "widget_metric_list.h"
#include "widget_metric_list_item.h"
#include "metrics_view.h"

void view_process(char key)
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

    ui_tree_process_event(event);
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

    unsigned screen_selector = 0;

    ui_node_desc_t ui = {
        .widget = &__widget_screen_switch,
        .cfg = &(__widget_screen_switch_cfg_t){
            .selector_ptr = &screen_selector,
            .screens_num = 5,
            .screens_list = (ui_node_desc_t[]){
                {
                    .widget = &__widget_test_rect_with_text,
                    .cfg = &(__widget_test_rect_with_text_cfg_t){
                        .text = "Watch screen",
                        .cs = {
                            .bg = 0xA04020,
                            .fg = 0x23fa75
                        }
                    }
                },
                {
                    .widget = &__widget_test_rect_with_text,
                    .cfg = &(__widget_test_rect_with_text_cfg_t){
                        .text = "Dashboard",
                        .cs = {
                            .bg = 0x708090,
                            .fg = 0xfffa75
                        }
                    }
                },
                {
                    .widget = &__widget_titled_screen,
                    .cfg = &(__widget_titled_screen_cfg_t){
                        .screen_num = 2,
                        .titles = (char * []){
                            "Debug real val list",
                            "Debug switches list",
                        },
                        .color_text = 0,
                        .color_bg_unselected = 0x678913,
                        .color_bg_selected = 0xfeea75,
                        .screen_list = (ui_node_desc_t []){
                            {
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
                            },
                            {
                                .widget = &__widget_metric_list,
                                .cfg = &(__widget_metric_list_cfg_t){
                                    .ui_node = &(ui_node_desc_t){
                                        .widget = &__widget_metric_list_item,
                                        .cfg = &(__widget_metric_list_item_cfg_t){
                                            .color_text = 0xfffa75,
                                            .color_value = 0x66CFAB,
                                            .color_raw = 0xaa4450,
                                            .color_unit = 0x00ff00,
                                            .color_bg = 0
                                        }
                                    },
                                    .borders = { .w = 4, .h = 4 },
                                    .gaps = { .x = 2, .y = 4 },
                                    .color_bg = 0,
                                    .num = METRIC_VAR_ID_NUM
                                }
                            }
                        }
                    }
                },
                {
                    .widget = &__widget_test_rect_with_text,
                    .cfg = &(__widget_test_rect_with_text_cfg_t){
                        .text = "Dump",
                        .cs = {
                            .bg = 0xA04020,
                            .fg = 0x23fa75
                        }
                    }
                },
                {
                    .widget = &__widget_test_rect_with_text,
                    .cfg = &(__widget_test_rect_with_text_cfg_t){
                        .text = "Settings",
                        .cs = {
                            .bg = 0xA04020,
                            .fg = 0x23fa75
                        }
                    }
                }
            }
        }
    };
    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, &ui, &emu_lcd_cfg.size);

    ui_tree_draw();

    ui_tree_debug_print_tree();

    emu_lcd_loop(view_process);

    return 0;
}
