#include "ui_tree.h"

#include "text_label_color.h"
#include "lcd_color.h"
#include "draw_color.h"
#include "val_mod.h"
#include "forms_split.h"

#include "event_list.h"

#include "widget_debug_metrics.h"

#include "widget_selectable_list.h"
#include "widget_screen_by_idx.h"

#include "widget_metric_list_item.h"
#include "widget_route_list_item.h"
#include "widget_tc_dump.h"

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
*/


static void widget_color_rect_draw(ui_element_t * el)
{
    static const lcd_color_t color_list[] = {
        0xAABBCC,
        0x11BBCC,
        0xAA22CC,
        0xAABB33,
    };
    const unsigned * color_idx = el->ui_node->cfg;
    draw_color_form(&el->f, color_list[el->idx]);
}

const widget_desc_t widget_color_rect = {
    .draw = widget_color_rect_draw
};

enum {
    MENU_METRICS,
    MENU_ROUTES,
    MENU_DUMP,
    MENU_SETTINGS
};

ui_node_desc_t menu_screens[] = {
    [MENU_METRICS] = {
        .widget = &widget_screen_by_idx,
        .cfg = &(widget_screen_by_idx_cfg_t){
            .screens_num = 2,
            .screens_list = (ui_node_desc_t[]){
                {
                    .widget = &widget_selectable_list,
                    .cfg = &(widget_selectable_list_cfg_t){
                        .margin = { .x = 0, .y = 2 },
                        .num = METRIC_VAR_ID_NUM,
                        .ui_node = &(ui_node_desc_t){
                            .widget = &widget_metric_list_item,
                            .cfg = &(widget_metric_list_item_cfg_t) {
                                .type = METRIC_LIST_VAL,
                            }
                        },
                    }
                },
                {
                    .widget = &widget_selectable_list,
                    .cfg = &(widget_selectable_list_cfg_t){
                        .margin = { .x = 0, .y = 2 },
                        .num = METRIC_BOOL_ID_NUM,
                        .ui_node = &(ui_node_desc_t){
                            .widget = &widget_metric_list_item,
                            .cfg = &(widget_metric_list_item_cfg_t) {
                                .type = METRIC_LIST_BOOL,
                            }
                        },
                    }
                },
            },
        }
    },
    [MENU_ROUTES] = {
        .widget = &widget_screen_by_idx,
        .cfg = &(widget_screen_by_idx_cfg_t){
            .screens_num = 1,
            .screens_list = (ui_node_desc_t[]){
                {
                    .widget = &widget_selectable_list,
                    .cfg = &(widget_selectable_list_cfg_t){
                        .num = ROUTE_TYPE_NUM,
                        .margin = { .x = 2, .y = 2 },
                        .ui_node = &(ui_node_desc_t){
                            .widget = &widget_route_list_item,
                            // .cfg = &(widget_metric_list_item_cfg_t) {
                            //     .type = METRIC_LIST_VAL,
                            // }
                        },
                    }
                },
            },
        }
    },
    [MENU_DUMP] = {
        .widget = &widget_dlc_dump
    },
    [MENU_SETTINGS] = {
        .widget = &widget_color_rect
    }
};

const lp_color_t title = {
    .color = 0,
    .l = {
        .xy = { .x = 2 },
        .len = 8,
        .text_list = (const char * []){
            [MENU_METRICS] =    "METRICS",
            [MENU_ROUTES] =     "ROUTES",
            [MENU_DUMP] =       "DLC DUMP",
            [MENU_SETTINGS] =   "SETTINGS"
        },
        .t = LP_T_LIDX
    }
};

const lp_color_t local_title[] = {
    [MENU_METRICS] =    { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text_list = (const char * []){ "REAL", "BOOL" }, .t = LP_T_LIDX } },
    [MENU_ROUTES] =     { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text_list = (const char * []){ "ACTUAL", "TRIP", "REFILL" }, .t = LP_T_LIDX } },
    [MENU_DUMP] =       { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text_list = (const char * []){ "ECU", "SRS", "ABS" }, .t = LP_T_LIDX } },
    [MENU_SETTINGS] =   { .color = 0x130e06, .l = { .xy = { .x = -3 }, .len = 8, .text_list = (const char * []){ "DATETIME", "ODO", "ANALOG" }, .t = LP_T_LIDX } },
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

struct menu_cfg {
    const widget_desc_t * widget;
    uint8_t menu_count;
    uint8_t * list_len;
    uint8_t confirm;
};

const struct menu_cfg menu_cfg[] = {
    [MENU_METRICS] =    { .widget = &widget_metric_list_item, .menu_count = 2, .list_len = (uint8_t []){ METRIC_VAR_ID_NUM, METRIC_BOOL_ID_NUM} },
    [MENU_ROUTES] =     { .widget = &widget_route_list_item,  .menu_count = 3, .list_len = (uint8_t []){ ROUTE_TYPE_NUM, ROUTE_TYPE_NUM, ROUTE_TYPE_NUM } },
    [MENU_DUMP] =       { .widget = &widget_color_rect, .menu_count = 3 },
    [MENU_SETTINGS] =   { .widget = &widget_color_rect, .menu_count = 3 },
};

// static const lcd_color_t bg = 0x4585E1;
static const lcd_color_t bg = 0xf08400;

static void select_update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    lp(&ctx->title_ctx, &local_title[el->idx].l, &(color_scheme_t){ .fg = local_title[el->idx].color, .bg = bg }, 0, 0, ctx->selector);
    lp_color(&ctx->title_ctx, 0x4585E1, &title_selector, el->active, 0, 0);
}
static const unsigned selector = 1;

static void redraw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    select_update(el);

    ui_element_t * item = ui_tree_add(el, &menu_screens[el->idx], ctx->selector);
    form_cut(&item->f, DIMENSION_Y, EDGE_U, ctx->title_form.s.h);

    ui_tree_element_draw(item);
    ui_tree_debug_print_tree();
}

static void change_cild(ui_element_t * el)
{
    ui_tree_delete_childs(el);
    redraw(el);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    ctx->selector = 0;
    ctx->title_form = el->f;
    tf_ctx_calc(&ctx->title_ctx, &ctx->title_form, &screen_title);
    draw_color_form(&ctx->title_form, bg);

    lp(&ctx->title_ctx, &title.l, &(color_scheme_t){ .fg = title.color, .bg = bg }, 0, 0, el->idx);

    redraw(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    if (event == EVENT_BTN_DOWN) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_ADD, 1, 0, menu_cfg[el->idx].menu_count - 1, 1);
        change_cild(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_SUB, 1, 0, menu_cfg[el->idx].menu_count - 1, 1);
        change_cild(el);
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
    .select = select_update,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
