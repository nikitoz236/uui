#include "ui_tree.h"
#include "tc_events.h"
#include "widget_test_rect_with_text.h"

#include "widget_screen_switch.h"
#include "widget_debug_metrics.h"

uint8_t screen_selector = 0;

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
                .widget = &__widget_screen_debug_metrics,
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

xy_t lcd_size = {
    .w = 320,
    .h = 240
};


void view_process(unsigned int event)
{
    ui_tree_process_event(event);
}

void view_screen_on(void)
{
    // lcd on
    ui_tree_init(ui_ctx, 1024, &ui, &lcd_size);
    ui_tree_draw();
    // ui_tree_debug_print_tree();
}

void view_screen_off(void);

void view_idle_screen(void);
void view_restore_screen(void);
