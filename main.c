#include <stdio.h>
#include "emu_common.h"

#include "api_lcd_color.h"

#include "widget_union.h"
#include "widget_emu_lcd.h"
#include "widget_text_color.h"

uint8_t ui_stack[2048];
uint8_t app_ui_stack[2048];

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

void __rect_widget_draw(ui_ctx_t * node_ctx)
{
    emu_draw_rect(&node_ctx->f, ((rect_widget_cfg_t *)node_ctx->node->widget_cfg)->color);
};

widget_t rect_widget = {
    .calc = __rect_widget_calc,
    .draw = __rect_widget_draw
};


void __rect_widget_draw_lcd(ui_ctx_t * node_ctx)
{
    lcd_rect(node_ctx->f.p.x, node_ctx->f.p.y, node_ctx->f.s.w, node_ctx->f.s.h, ((rect_widget_cfg_t *)node_ctx->node->widget_cfg)->color);
};

widget_t rect_widget_lcd = {
    .calc = __rect_widget_calc,
    .draw = __rect_widget_draw_lcd
};

#define WIDGET_RECT(sw, sh, c) \
    { \
        .widget = &rect_widget, \
        .widget_cfg = &(rect_widget_cfg_t){ \
            .size = { \
                .w = sw, \
                .h = sh \
            }, \
            .color = c \
        } \
    }

#define WBUTTON(c)         WIDGET_RECT( 100, 100, c )

#define WLCD \
    { \
        .widget = &__widget_emu_lcd, \
        .widget_cfg = &(widget_emu_lcd_cfg_t){ \
            .size = { \
                .w = 96, \
                .h = 68 \
            }, \
            .scale = 7, \
            .px_gap = 1, \
            .border = 20, \
            .bg_color = 0x5f7537 \
        } \
    }



ui_node_t ui_test =
    WIDGET_UNION( RO, 0, C, 0,
        WIDGET_UNION( LO, 0, C, 0,
            WLCD,
            WIDGET_UNION( C, 0, DI, 30,
                WBUTTON(0x00009F),
                WBUTTON(0x0000FF)
            )
        ),
        WIDGET_UNION( C, 0, DI, 30,
            WBUTTON(0x00002F),
            WBUTTON(0x00002F)
        )
    )
;

    #include "widget_list_scroll.h"


int main() {
    ui_ctx_t * ui_ctx = (ui_ctx_t *)ui_stack;
    ui_node_t * node = &ui_test;
    calc_node(node, ui_ctx, 0);
    gfx_open(ui_ctx->f.s.x, ui_ctx->f.s.y, "UC lib gfx emu device");

    widgets_print_ctx(0, ui_ctx, 8);

    draw_node(ui_ctx);
    printf("\n\nui ctx struct size: %ld\r\n", sizeof(ui_ctx_t));
    printf("ui ctx size: %d\r\n", ui_ctx->size);
    int size = ui_ctx->size;

    // void * ctx = ui_ctx;
    // while (size > 0) {
    //     ui_ctx_t * ui_ctx_ptr = ctx;
    //     int len = ui_ctx_ptr->size;
    //     printf("node: size: %d, idx: %d, ", len, ui_ctx_ptr->idx);
    //     printf("pos: (%d, %d), size: (%d, %d)\r\n",ui_ctx_ptr->f.p.x, ui_ctx_ptr->f.p.y, ui_ctx_ptr->f.s.w, ui_ctx_ptr->f.s.h);
    //     ctx += sizeof(ui_ctx_t);
    //     size -= sizeof(ui_ctx_t);
    // }

    lcd_rect(0, 0, 96, 68, 0x526137);
        // lcd_rect(10, 40, 30, 22, 0x0);

    lcd_color_t bitmap[] = {
        0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    };

    widgets_print_ctx(0, ui_ctx, 8);

    // lcd_image(50, 50, 2, 2, 4, bitmap);

    //     extern font_t font_5x7;
    //     extern font_t font_5x5;


    // const ui_node_t app_ui_root = {
    //     .widget = &__widget_text_color,
    //     .widget_cfg = &(widget_text_color_cfg_t){
    //         .text_size = { .w = 6, .h = 3 },
    //         .font = &font_5x7,
    //         .gaps = { .w = 3, .h = 4 },
    //         .scale = 1
    //     }
    // };

    // calc_node(&app_ui_root, (ui_ctx_t*)app_ui_stack, 0);
    // widget_text_color_ctx_t * text_ctx = (widget_text_color_ctx_t *)((ui_ctx_t*)app_ui_stack)->ctx;
    // text_ctx->cs = &(color_scheme_t){ .fg = 0x0000FF, .bg = 0x000000 },
    // text_ctx->text = "Hey bitch!";
    // draw_node(&app_ui_root, (ui_ctx_t*)app_ui_stack);

    const ui_node_t app_list = {
        .widget = &__widget_list_scroll,
        .widget_cfg = &(widget_list_scroll_cfg_t){
            .border = 10,
            .gap = 4,
            .el_len = 2,
            .selectable = 1,
            .stretch = 0,
            .node = &(ui_node_t){
                .widget = &rect_widget_lcd,
                .widget_cfg = &(rect_widget_cfg_t){
                    .size = {50, 4},
                    .color = 0x00FFFF
                }
            }
        }
    };

    calc_node(&app_list, (ui_ctx_t*)app_ui_stack, 0);
    ((ui_ctx_t*)app_ui_stack)->f.s = (xy_t){ 96, 65 };
    draw_node((ui_ctx_t*)app_ui_stack);

    while (1) {
        char kbd_btn = gfx_routine();

        if (kbd_btn != 0) {
            printf("recieved keyboard button: %c [%d]\r\n", kbd_btn, kbd_btn);

            if (kbd_btn == 'q') {
                break;
            }
        }
        usleep(1000);
    }

    return 0;
}
