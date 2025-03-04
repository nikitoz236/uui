#include "ui_tree.h"

#include "text_label_color.h"
#include "lcd_color.h"
#include "draw_color.h"
#include "val_mod.h"
#include "forms_split.h"

#include "event_list.h"

#include "widget_debug_metrics.h"
#include "widget_metric_list.h"
#include "widget_metric_list_item.h"

// #include "widget_titled_screen.h"
// #include "metrics_view.h"

extern font_t font_5x7;

text_field_t screen_title = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .x = 2, .y = 2 },
        .scale = 2
    },
    .limit_char = { .y = 1 },
    .a = ALIGN_LIC,
    .padding = { .x = 6, .y = 4 }
};

typedef struct {
    form_t title_form;
    tf_ctx_t title_ctx;
    unsigned selector;
} ctx_t;


/*
============================================
0 2         13
< METRICS   >

============================================


я могу создать дочерний элемент на остатке экрана

    у него будет


*/

const lp_color_t title = {
    .color = 0, .l = { .xy = { .x = 2 }, .len = 8, .text = (const char * []){ "METRICS", "ROUTES", "DLC DUMP", "SETTINGS" }, .t = LP_T_LIDX }
};

const lp_color_t local_title[] = {
    { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text = (const char * []){ "REAL", "BOOL" }, .t = LP_T_LIDX } },
    { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text = (const char * []){ "ACTUAL", "TRIP", "REFILL" }, .t = LP_T_LIDX } },
    { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text = (const char * []){ "ECU", "SRS", "ABS" }, .t = LP_T_LIDX } },
    { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text = (const char * []){ "DATETIME", "ODO", "ANALOG" }, .t = LP_T_LIDX } },
};

const label_list_t title_selector = {
    .wrap_list = (lp_color_t []) {
        { .color = 0, .l = { .xy = { .x =  0 }, .len = 1, .text_list = (const char *[]){ "[", 0 }, .t = LP_T_LIDX } },
        { .color = 0, .l = { .xy = { .x = 11 }, .len = 1, .text_list = (const char *[]){ "]", 0 }, .t = LP_T_LIDX } },
        { .color = 0, .l = { .xy = { .x = -12 }, .len = 1, .text_list = (const char *[]){ 0, "[" }, .t = LP_T_LIDX } },
        { .color = 0, .l = { .xy = { .x = -1 }, .len = 1, .text_list = (const char *[]){ 0, "]" }, .t = LP_T_LIDX } },
    },
    .count = 4
};


void widget_color_rect_draw(ui_element_t * el)
{
    const lcd_color_t * color = el->ui_node->cfg;
    draw_color_form(&el->f, *color);
}

const widget_desc_t widget_color_rect = {
    .draw = widget_color_rect_draw
};

const lcd_color_t color_list[] = {
    0xAABBCC,
    0x11BBCC,
    0xAA22CC,
    0xAABB33,
};

ui_node_desc_t menu_screens[] = {
    {
        .widget = &widget_color_rect,
        .cfg = &color_list[0]
    },
    {
        .widget = &widget_color_rect,
        .cfg = &color_list[1]
    },
    {
        .widget = &widget_color_rect,
        .cfg = &color_list[2]
    },
    {
        .widget = &widget_color_rect,
        .cfg = &color_list[3]
    },
};

// static const lcd_color_t bg = 0x4585E1;
static const lcd_color_t bg = 0xf08400;

static void redraw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    lp(&ctx->title_ctx, &local_title[el->idx].l, &(color_scheme_t){ .fg = local_title[el->idx].color, .bg = bg }, 0, 0, ctx->selector);
    lp_color(&ctx->title_ctx, bg, &title_selector, el->active, 0, 0);

    ui_tree_element_draw(ui_tree_child(el));
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    ctx->title_form = el->f;
    tf_ctx_calc(&ctx->title_ctx, &ctx->title_form, &screen_title);
    draw_color_form(&ctx->title_form, bg);

    lp(&ctx->title_ctx, &title.l, &(color_scheme_t){ .fg = title.color, .bg = bg }, 0, 0, el->idx);

    ui_element_t * item = ui_tree_add(el, &menu_screens[el->idx], ctx->selector);
    form_cut(&item->f, DIMENSION_Y, EDGE_U, ctx->title_form.s.h);

    redraw(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    const uint8_t count[] = { 2, 3, 3, 3 };

    if (event == EVENT_BTN_DOWN) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_ADD, 1, 0, count[el->idx] - 1, 1);
        redraw(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_SUB, 1, 0, count[el->idx] - 1, 1);
        redraw(el);
        return 1;
    }
    if (event == EVENT_BTN_OK) {
        return ui_tree_element_select(ui_tree_child(el), 1);
    }
    if (event == EVENT_BTN_BACK) {
        return ui_tree_element_select(ui_tree_child(el), 0);
    }
    return 0;
}

const widget_desc_t widget_screen_debug_metrics = {
    .draw = draw,
    .select = redraw,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
