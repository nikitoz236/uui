#include "widget_emu_lcd.h"
#include "emu_common.h"

#include "ui_tree.h"

#include "color_scheme_type.h"
#include "lcd_text_color.h"
#include "str_utils.h"

typedef struct {
    color_scheme_t cs;
    char * text;
} __widget_test_rect_with_text_cfg_t;

extern font_t font_5x7;

static inline void form_fill(form_t * f, lcd_color_t color)
{
    lcd_rect(f->p.x, f->p.y, f->s.w, f->s.h, color);
}

void draw_test_rect_with_text(ui_element_t * el)
{
    __widget_test_rect_with_text_cfg_t * cfg = (__widget_test_rect_with_text_cfg_t *)el->ui_node->cfg;
    form_fill(&el->f, cfg->cs.bg);
    lcd_text_cfg_t text_cfg = {
        .font = &font_5x7,
        .text_size = {str_len(cfg->text, 20), 1},
        .gaps = {1, 1},
        .scale = 1
    };

    lcd_text_extend_scale(&el->f.s, &text_cfg);

    printf("text cfg: text size: [%d, %d] gaps: [%d, %d] scale: %d\n", text_cfg.text_size.w, text_cfg.text_size.h, text_cfg.gaps.x, text_cfg.gaps.y, text_cfg.scale);

    form_t text_form;
    lcd_text_calc_size(&text_form.s, &text_cfg);
    form_align(&el->f, &text_form, &ALIGN_MODE(C, 0, C, 0));
    lcd_text_color_print(cfg->text, &text_form.p, &text_cfg, &cfg->cs, 0, 0, 0);
}

widget_desc_t __widget_test_rect_with_text = {
    .draw = draw_test_rect_with_text,
};


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
