#include "widget_settings_title.h"
#include "text_label_color.h"
#include "draw_color.h"
#include "forms_split.h"

extern font_t font_5x7;

const tf_cfg_t menu_title_cfg = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .x = 2, .y = 2 },
        .scale = 2
    },
    .limit_char = { .y = 1 },
    .a = ALIGN_LIC,
    .padding = { .x = 6, .y = 4 }
};

static const lcd_color_t bg = COLOR(112233);

typedef struct {
    tf_ctx_t title;
} ctx_t;

static const label_list_t title_selector = {
    .wrap_list = (label_color_t []) {
        { .color = COLOR(0x11ff44), .l = { .xy = { .x =  0 }, .len = 1, .text_list = (const char *[]){ 0, "[" }, .t = LP_T_LIDX } },
        { .color = COLOR(0x11ff44), .l = { .xy = { .x = 12 }, .len = 1, .text_list = (const char *[]){ 0, "]" }, .t = LP_T_LIDX } },
    },
    .count = 2
};

static void select(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    label_color_list(&ctx->title, &title_selector, bg, el->active, 0, 0);
    ui_tree_element_select(ui_tree_child(el), el->active);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_settings_title_cfg_t * cfg = (widget_settings_title_cfg_t *)el->ui_node->cfg;
    form_t f_title = el->f;
    // тут надо просто сначала одну форму посчитать, потом дочернего элмемента и склеить их размеры
    if (tf_ctx_calc(&ctx->title, &f_title, &menu_title_cfg)) {

        ui_element_t * item = ui_tree_add(el, cfg->screen, el->idx);
        form_cut(&item->f, DIMENSION_Y, EDGE_U, f_title.s.h);
        ui_tree_element_draw(item);

        if (item->drawed) {
            form_reduce(&el->f, DIMENSION_Y, EDGE_U, f_title.s.h + item->f.s.h);

            draw_color_form(&f_title, bg);
            // draw_color_form(&el->f, bg);

            // label_color_t title_label = { .color = COLOR(0x11ff44), .l = { .text_list = cfg->titlet_list, .xy = { .x = 2 }, .t = LP_T_LIDX } };
            // memset

            label_color_t title_label;
            title_label.color = COLOR(0x11ff44);
            title_label.l.text_list = cfg->title_list;
            title_label.l.t = LP_T_LIDX;
            title_label.l.xy.x = 2;
            title_label.l.xy.y = 0;



            label_color(&ctx->title, &title_label, bg, el->idx, 0, 0);
            select(el);

            el->drawed = 1;
        }
    }
}

const widget_desc_t widget_settings_title = {
    .select = select,
    .draw = draw,
    .ctx_size = sizeof(ctx_t)
};
