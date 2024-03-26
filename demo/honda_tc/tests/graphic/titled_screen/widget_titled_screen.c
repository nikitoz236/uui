#include "widget_titled_screen.h"
#include "lcd_text_color.h"

#include "fonts.h"
#include "str_utils.h"

extern font_t font_5x7;

#define MARGIN 2

static inline void form_fill(form_t * f, lcd_color_t color)
{
    lcd_rect(f->p.x, f->p.y, f->s.w, f->s.h, color);
}

static inline void create_title(char * title, color_scheme_t * cs, form_t * pf, form_t * sf)
{
/*
    мне нужно создать формочку для текста, и отрезать ей остальное для вложеного элемента

    у меня есть размер который нужен для текста
    плюс я хочу с двух сторон сделать отступ
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

static void draw(ui_element_t * el)
{
    __widget_titled_screen_cfg_t * cfg = (__widget_titled_screen_cfg_t *)el->ui_node->cfg;
    ui_element_t * item = ui_tree_add(el, cfg->screen, 0);
    create_title(cfg->title, &cfg->cs, &el->f, &item->f);
    ui_tree_element_draw(item);
}

widget_desc_t __widget_titled_screen = {
    .draw = draw,
};
