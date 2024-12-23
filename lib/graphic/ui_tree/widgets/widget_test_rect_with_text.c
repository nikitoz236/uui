#include "widget_test_rect_with_text.h"
#include "api_lcd_color.h"

#include "color_scheme_type.h"
#include "lcd_text_color.h"
#include "str_utils.h"
#include "align_forms.h"
#include "draw_color.h"

extern font_t font_5x7;

static void draw(ui_element_t * el)
{
    __widget_test_rect_with_text_cfg_t * cfg = (__widget_test_rect_with_text_cfg_t *)el->ui_node->cfg;
    draw_color_form(&el->f, cfg->cs.bg);
    lcd_text_cfg_t text_cfg = {
        .font = &font_5x7,
        .text_size = {str_len(cfg->text, 20), 1},
        .gaps = {1, 1},
        .scale = 1
    };

    lcd_text_extend_scale(&el->f.s, &text_cfg);

    form_t text_form;
    lcd_text_calc_size(&text_form.s, &text_cfg);
    form_align(&el->f, &text_form, &ALIGN_MODE(C, 0, C, 0));
    lcd_text_color_print(cfg->text, &text_form.p, &text_cfg, &cfg->cs, 0, 0, 0);
}

const widget_desc_t __widget_test_rect_with_text = {
    .draw = draw,
};
