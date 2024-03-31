#include "widget_titled_screen.h"
#include "color_forms.h"
#include "lcd_text_color.h"

#include "fonts.h"
#include "str_utils.h"

extern font_t font_5x7;

#define MARGIN 2

typedef struct {
    uint8_t current_idx;
    uint8_t focus;
} __widget_titled_screen_ctx_t;

static inline void create_title(char * title, color_scheme_t * cs, form_t * pf, form_t * sf)
{
    /*
        также посчитает размер формы sf оставшегося доступного окна
    */

    lcd_text_cfg_t text_cfg = {
        .font = &font_5x7,
        .text_size = { .w = str_len(title, 20), .h = 1 },
        .gaps = { 1, 1},
        .scale = 1
    };

    form_t text_form;
    lcd_text_calc_size(&text_form.s, &text_cfg);

    form_t text_box = *pf;
    form_cut(&text_box, text_form.s.h + (2 * MARGIN), DIMENSION_HEIGHT, EDGE_D);

    form_fill(&text_box, cs->bg);

    form_align(pf, &text_form, &ALIGN_MODE(LI, 6, UI, MARGIN));
    lcd_text_color_print(title, &text_form.p, &text_cfg, cs, 0, 0, 0);

    *sf = *pf;
    form_cut(sf, text_form.s.h + (2 * MARGIN), DIMENSION_HEIGHT, EDGE_U);
}

#include "str_val.h"

static void update(ui_element_t * el)
{
    __widget_titled_screen_ctx_t * ctx = (__widget_titled_screen_ctx_t *)el->ctx;
    __widget_titled_screen_cfg_t * cfg = (__widget_titled_screen_cfg_t *)el->ui_node->cfg;

    unsigned screen_idx = ctx->current_idx;

    ui_element_t * item = ui_tree_add(el, &cfg->screen_list[screen_idx], 0);
    color_scheme_t cs;
    cs.fg = cfg->color_text;
    if (ctx->focus) {
        cs.bg = cfg->color_bg_selected;
    } else {
        cs.bg = cfg->color_bg_unselected;
    }

    create_title(cfg->titles[screen_idx], &cs, &el->f, &item->f);
    ui_tree_element_draw(item);
}


static void draw(ui_element_t * el)
{
    __widget_titled_screen_cfg_t * cfg = (__widget_titled_screen_cfg_t *)el->ui_node->cfg;
    __widget_titled_screen_ctx_t * ctx = (__widget_titled_screen_ctx_t *)el->ctx;

    ctx->current_idx = 0;
    ctx->focus = 0;

    update(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    __widget_titled_screen_cfg_t * cfg = (__widget_titled_screen_cfg_t *)el->ui_node->cfg;
    __widget_titled_screen_ctx_t * ctx = (__widget_titled_screen_ctx_t *)el->ctx;

    if (ctx->focus) {
        ui_element_t * item = ui_tree_child(el);
        if (ui_tree_element_process(item, event)) {
            return 1;
        }
        if (event == 3) {
            ctx->current_idx++;
            if (ctx->current_idx == cfg->screen_num) {
                ctx->current_idx = 0;
            }
            update(el);
            return 1;
        }
        if (event == 4) {
            if (ctx->current_idx == 0) {
                ctx->current_idx = cfg->screen_num;
            }
            ctx->current_idx--;
            update(el);
            return 1;
        }
    } else {
        // а если мы провалились в дочерний элемент, и там есть свой фокус ? нам надо из него выйти
    }
    if (event == 10) {
        ctx->focus = !ctx->focus;
        update(el);
        return 1;
    }
    return 0;
}

widget_desc_t __widget_titled_screen = {
    .draw = draw,
    .process_event = process,
    .ctx_size = sizeof(__widget_titled_screen_ctx_t)
};
