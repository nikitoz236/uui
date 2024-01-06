#include "widget_grid.h"
#include "widget_emu_lcd.h"
#include "emu_common.h"
#include <stdio.h>

typedef struct {
    xy_t size;
    unsigned color;
} rect_widget_cfg_t;

unsigned __rect_widget_calc(ui_ctx_t * node_ctx)
{
    node_ctx->child_offset = 0;
    node_ctx->f.s = ((rect_widget_cfg_t *)node_ctx->node->widget_cfg)->size;
    return 0;
};

void __rect_widget_draw_lcd(ui_ctx_t * node_ctx)
{
    lcd_rect(node_ctx->f.p.x, node_ctx->f.p.y, node_ctx->f.s.w, node_ctx->f.s.h, ((rect_widget_cfg_t *)node_ctx->node->widget_cfg)->color);
};

widget_t rect_widget_lcd = {
    .calc = __rect_widget_calc,
    .draw = __rect_widget_draw_lcd,
    .stretched = 1
};

int main()
{
    printf("widget grid test\r\n");

    ui_node_t emu_root = {
        .widget = &__widget_emu_lcd,
        .widget_cfg = &(widget_emu_lcd_cfg_t){
            .size = { .w = 320, .h = 240 },
            .bg_color = 0,
            .border = 10,
            .px_gap = 1,
            .scale = 2
        }
    };

    uint8_t emu_ui_stack[1024];
    ui_ctx_t * emu_ui_ctx = (ui_ctx_t *)emu_ui_stack;

    calc_node(&emu_root, emu_ui_ctx, 0);
    emu_ui_ctx->f.p = (xy_t){0, 0};
    gfx_open(emu_ui_ctx->f.s.w, emu_ui_ctx->f.s.h, "widget grid test");

    ui_node_t root = {
        .widget = &__widget_grid,
        .widget_cfg = &(widget_grid_cfg_t){
            .grid_cfg = {
                .ca[0] = {
                    .count = 2,
                    .gap = 4
                },
                .ca[1] = {
                    .count = 4,
                    .gap = 4
                }
            },
            .childs = (grid_child_elements_t[]){
                {
                    .node = &(ui_node_t){
                        .widget = &rect_widget_lcd,
                        .widget_cfg = &(rect_widget_cfg_t){
                            .size = { .w = 100, .h = 20 },
                            .color = 0xFFff00
                        }
                    },
                    .count = 7
                },
                {
                    .node = &(ui_node_t){
                        .widget = &__widget_grid,
                        .widget_cfg = &(widget_grid_cfg_t){
                            .grid_cfg = {
                                .ca[0] = {
                                    .count = 2,
                                    .gap = 4
                                },
                                .ca[1] = {
                                    .count = 2,
                                    .gap = 4
                                }
                            },
                            .childs = (grid_child_elements_t[]){
                                {
                                    .node = &(ui_node_t){
                                        .widget = &rect_widget_lcd,
                                        .widget_cfg = &(rect_widget_cfg_t){
                                            .size = { .w = 60, .h = 20 },
                                            .color = 0xFF00FF
                                        }
                                    },
                                    .count = 4
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    uint8_t ui_stack[1024];
    ui_ctx_t * ui_ctx = (ui_ctx_t *)ui_stack;
    calc_node(&root, ui_ctx, 0);
    widgets_print_ctx(0, ui_ctx, 16);
    ui_ctx->f = (form_t){.p = {0, 0}, .s = {320, 240}};

    draw_node(ui_ctx);

    while (gfx_routine() != 'q') {};

    return 0;
}