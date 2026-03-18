#include "widget__test_selectable_text.h"
#include "draw_color.h"
#include "lcd_text_color.h"

extern const font_t font_5x7;

static const lcd_font_cfg_t fcfg = {
    .font = &font_5x7,
};

#define TEXT_PADDING 4

static void draw(ui_element_t * el)
{
    WIDGET_IMPORT_CFG(widget_cfg__test_selectable_text_t, cfg);
    lcd_color_t color = cfg->colors[el->flags.focused ? 1 : 0];
    draw_frame(&el->f, 2, color);

    tptr_t t = text_ptr_create(tf_create(&fcfg, el->f, (xy_t){ .x = TEXT_PADDING, .y = TEXT_PADDING }));
    color_scheme_t cs = { .fg = color, .bg = 0x000000 };
    lcd_color_tptr_print(&t, cfg->text, cs, 0);
}

static void on_select(ui_element_t * el)
{
    draw(el);
}

const widget_desc_t widget__test_selectable_text = {
    .ctx_size = WIDGET_CTX_NONE,
    .draw = draw,
    .select = on_select,
};
