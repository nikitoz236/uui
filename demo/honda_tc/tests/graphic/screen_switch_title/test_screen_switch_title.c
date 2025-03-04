#include <stdio.h>
#include "emu_tc.h"

#include "widget_screen_switch.h"
#include "event_list.h"

#include "draw_color.h"

extern font_t font_5x7;
uint8_t screen_selector = 0;

void widget_color_rect_draw(ui_element_t * el)
{
    lcd_color_t * color = el->ui_node->cfg;
    draw_color_form(&el->f, *color);
}

const widget_desc_t widget_color_rect = {
    .draw = widget_color_rect_draw
};

const lcd_color_t color_list[] = {
    0xAABBCC,
    0x11BBCC,
    0xAA22CC,
    0xAABB33,
};

ui_node_desc_t ui = {
    .widget = &widget_screen_switch,
    .cfg = &(widget_screen_switch_cfg_t){
        .screens_num = 4,
        .title_cfg = &(text_field_t){
            .fcfg = &(lcd_font_cfg_t){
                .font = &font_5x7,
                .gaps = { .x = 2, .y = 2 },
                .scale = 2
            },
            .limit_char = { .y = 1 },
            .a = ALIGN_LIC,
            .padding = { .x = 2, .y = 2 }
        },
        .titles = (const char *[]){
            "Widget",
            "Debug Metrics Bool",
            "Debug Metrics REAL",
            "DUMP ECU",
        },
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &widget_color_rect,
                .cfg = &color_list[0]
            },
            {
                .widget = &widget_color_rect,
                .cfg = &color_list[1]
            },
            {
                .widget = &widget_color_rect,
                .cfg = &color_list[2]
            },
            {
                .widget = &widget_color_rect,
                .cfg = &color_list[3]
            },
        }
    }
};


int main()
{
    printf("test widget_screen_switch titled\r\n");
    emu_ui_node(&ui);
    return 0;
}
