#include <stdio.h>
#include "emu_tc.h"

#include "lcd_text.h"
#include "lcd_text_color.h"
#include "align_forms.h"
#include "str_utils.h"
#include "draw_color.h"

#include "widget_selectable_list.h"

typedef struct {
    lcd_color_t color_bg_unselected;
    lcd_color_t color_bg_selected;
    lcd_color_t color_text;
    const font_t * font;
    char * text;
    uint8_t scale;
} widget_text_cfg_t;

typedef struct {
    xy_t pos;
    lcd_text_cfg_t text_cfg;
} widget_text_ctx_t;

void update(ui_element_t * el)
{

}

void draw_widget_text(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    widget_text_ctx_t * ctx = (widget_text_ctx_t *)el->ctx;

    color_scheme_t cs = { .fg = cfg->color_text };
    if (el->active) {
        cs.bg = cfg->color_bg_selected;
        printf("draw selected text %s\n", cfg->text);
    } else {
        cs.bg = cfg->color_bg_unselected;
        printf("draw unselected text %s\n", cfg->text);
    }
    draw_color_form(&el->f, cs.bg);
    lcd_text_color_print(cfg->text, &ctx->pos, &ctx->text_cfg, &cs, 0, 0, 0);
}

void select(ui_element_t * el, unsigned selected)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    widget_text_ctx_t * ctx = (widget_text_ctx_t *)el->ctx;

    el->active = selected;

    draw_widget_text(el);
}

void calc(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    widget_text_ctx_t * ctx = (widget_text_ctx_t *)el->ctx;

    ctx->text_cfg.font = cfg->font;
    ctx->text_cfg.text_size = (xy_t){ .x = str_len(cfg->text, 20), .y = 1};
    ctx->text_cfg.gaps = (xy_t){.x = cfg->scale, .y = cfg->scale};
    ctx->text_cfg.scale = cfg->scale;

    lcd_text_calc_size(&el->f.s, &ctx->text_cfg);

    el->f.s.h += 2 * cfg->scale;        // зазоры по вертикали
}

void draw(ui_element_t * el)
{
    widget_text_cfg_t * cfg = (widget_text_cfg_t *)el->ui_node->cfg;
    widget_text_ctx_t * ctx = (widget_text_ctx_t *)el->ctx;

    // lcd_text_extend_scale(&el->f.s, &ctx->text_cfg);

    form_t text_form;
    lcd_text_calc_size(&text_form.s, &ctx->text_cfg);
    form_align(&el->f, &text_form, &ALIGN_MODE(LI, 5, C, 0));
    ctx->pos = text_form.p;

    draw_widget_text(el);
}

widget_desc_t __widget_text = {
    .calc = calc,
    .draw = draw,
    .select = select,
    .ctx_size = sizeof(widget_text_ctx_t)
};

extern font_t font_5x7;

ui_node_desc_t ui = {
    .widget = &__widget_selectable_list,
    .cfg = &(__widget_selectable_list_cfg_t) {
        .num = 11,
        .different_nodes = 0,
        .ui_node = (ui_node_desc_t[]) {
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "Hello, world!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "hey bitch!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "fuck you!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate1!",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!2",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!3",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!4",
                    .scale = 2
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "snow good!",
                    .scale = 4
                }
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!5",
                    .scale = 2
                }
            },            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i want chocolate!6",
                    .scale = 2
                }
                
            },
            {
                .widget = &__widget_text,
                .cfg = &(widget_text_cfg_t) {
                    .color_bg_unselected = COLOR_BG_UNSELECTED,
                    .color_bg_selected = COLOR_BG_SELECTED,
                    .color_text = COLOR_TEXT,
                    .font = &font_5x7,
                    .text = "i like tits!",
                    .scale = 3
                }
            },
        }
    }
};

int main()
{
    printf("test widget_selectable_list\r\n");
    emu_ui_node(&ui);
    return 0;
}
