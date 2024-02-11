#include "emu_lcd.h"

#include "widget_test_rect_with_text.h"
#include "widget_layout_tree.h"

void view_process(char key)
{
    unsigned event = 0;
    if (key == 'h') {
        event = 1;
    } else if (key == 'l') {
        event = 2;
    }

    ui_tree_process_event(event);
}

int main()
{
    printf("test widget test\r\n");

    __widget_emu_lcd_cfg_t emu_lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .bg_color = 0x202020,
        .border = 10,
        .px_gap = 0,
        .scale = 3
    };

    emu_lcd_init(&emu_lcd_cfg);

    uint8_t layout_selector = 2;

    ui_node_desc_t ui = {
        .widget = &__widget_layout_tree,
        .cfg = &(__widget_layout_tree_cfg_t){
            .selected_layout = &layout_selector,
            .size_list = (xy_t[]){
                {1, 1},
                {1, 2},
                {2, 2},
                {2, 4}
            },
            .size_list_len = 4,
            .node_list = (ui_node_desc_t[]){
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "first" } },
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "second" } },
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "third" } },
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "four" } },
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "five" } },
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "six" } },
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "seven" } },
                { .widget = &__widget_test_rect_with_text, .cfg = &(__widget_test_rect_with_text_cfg_t){ .text = "eight" } }
            },
            .node_list_len = 8,
            .gaps = { .x = 2, .y = 2 },
            .borders = { .x = 4, .y = 4 },
        }
    };

    for (unsigned i = 0; i < ((__widget_layout_tree_cfg_t*)ui.cfg)->node_list_len; i++) {
        __widget_test_rect_with_text_cfg_t * cfg = (__widget_test_rect_with_text_cfg_t*)((__widget_layout_tree_cfg_t*)ui.cfg)->node_list[i].cfg;
        unsigned bg = rand() & 0xffffff;
        unsigned fg = ~bg & 0xffffff;
        cfg->cs.fg = fg;
        cfg->cs.bg = bg;
    }

    uint8_t ui_ctx[1024];
    ui_tree_init(ui_ctx, 1024, &ui, &emu_lcd_cfg.size);
    ui_tree_draw();
    emu_lcd_loop(view_process);

    return 0;
}
