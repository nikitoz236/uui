#include "widget_emu_lcd.h"
#include "emu_common.h"

#include "widget_test_rect_with_text.h"

int main()
{
    printf("test widget test\r\n");

    ui_node_t emu_root = {
        .widget = &__widget_emu_lcd,
        .widget_cfg = &(widget_emu_lcd_cfg_t){
            .size = { .w = 320, .h = 240 },
            .bg_color = 0x202020,
            .border = 10,
            .px_gap = 0,
            .scale = 3
        }
    };

    uint8_t emu_ui_stack[1024];
    ui_ctx_t * emu_ui_ctx = (ui_ctx_t *)emu_ui_stack;
    calc_node(&emu_root, emu_ui_ctx, 0);
    emu_ui_ctx->f.p = (xy_t){0, 0};
    gfx_open(emu_ui_ctx->f.s.w, emu_ui_ctx->f.s.h, "test widget test");
    draw_node(emu_ui_ctx);

    // init_view();

    ui_node_desc_t ui = {
        .widget = &__widget_test_rect_with_text,
        .cfg = &(__widget_test_rect_with_text_cfg_t){
            .cs = {
                .bg = 0x04ff00,
                .fg = 0x120033
            },
            .text = "Hello, world!"
        }
    };


    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, &ui, &((widget_emu_lcd_cfg_t*)emu_root.widget_cfg)->size);

    ui_tree_draw();

    while (1) {
        char key = gfx_routine();
        if (key == 'q') {
            break;
        }

        unsigned event = 0;
        if (key == 'h') {
            event = 1;
        } else if (key == 'l') {
            event = 2;
        }


        // view_process(event);

        usleep(250000);
    }

    return 0;
}
