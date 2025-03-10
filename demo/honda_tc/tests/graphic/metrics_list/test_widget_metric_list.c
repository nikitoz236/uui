#include <stdio.h>
#include "emu_tc.h"

#include "widget_screen_switch.h"
#include "widget_selectable_list.h"
#include "widget_metric_list_item.h"

extern font_t font_5x7;

ui_node_desc_t ui = {
    .widget = &widget_screen_switch,
    .cfg = &(widget_screen_switch_cfg_t){
        .screens_num = 2,
        .title_cfg = &(tf_cfg_t){
            .fcfg = &(lcd_font_cfg_t){
                .font = &font_5x7,
                .gaps = { .x = 2, .y = 2 },
                .scale = 2
            },
            .limit_char = { .y = 1 },
            .a = ALIGN_LIUI,
            .padding = { .x = 2, .y = 2 }
        },
        .titles = (const char *[]){
            "Debug Metrics Real",
            "Debug Metrics Bool"
        },
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &widget_selectable_list,
                .cfg = &(widget_selectable_list_cfg_t){
                    .ui_node = &(ui_node_desc_t){
                        .widget = &widget_metric_list_item,
                        .cfg = &(widget_metric_list_item_cfg_t){
                            .type = METRIC_LIST_VAL,
                        }
                    },
                    .margin = { .x = 0, .y = 0 },
                    .num = METRIC_VAR_ID_NUM,
                }
            },
            {
                .widget = &widget_selectable_list,
                .cfg = &(widget_selectable_list_cfg_t){
                    .ui_node = &(ui_node_desc_t){
                        .widget = &widget_metric_list_item,
                        .cfg = &(widget_metric_list_item_cfg_t){
                            .type = METRIC_LIST_VAL,
                        }
                    },
                    .margin = { .x = 0, .y = 0 },
                    .num = METRIC_BOOL_ID_NUM,
                }
            },
        }
    }
};

// from red obd project
#define OBD_SPEED_KMH			123
#define OBD_RPM					4567

const uint8_t obd_test_map_ecu[] = {
    (OBD_RPM / 64),						// 0x00		rpm * 4 high byte
    ((OBD_RPM % 64) * 4),				// 0x01		rpm * 4 low byte
    OBD_SPEED_KMH,						// 0x02		speed km/h
                      0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xA1, 0x00, 0x00, 0x01,
    0x29, 0x6C, 0x30, 0x00, 0x19, 0x13, 0x00, 0x93, 0x84, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
    0x83, 0x00, 0x80, 0x00, 0x02, 0x74, 0x54, 0x00, 0x17, 0x61, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x2D, 0x37, 0x00, 0x2D, 0x07, 0x2D, 0x19, 0x19, 0x19, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0xB8, 0x10, 0x96, 0x0B, 0x10, 0x10, 0x00, 0x07, 0x00
};

int main()
{
    printf("test_widget_metric_list\r\n");

    for (unsigned a = 0; a < sizeof(obd_test_map_ecu); a += 16) {
        metric_ecu_data_ready(a, &obd_test_map_ecu[a], 16);
    }

    emu_ui_node(&ui);
    return 0;
}
