#include "ui_tree.h"

#include "text_label_color.h"
#include "lcd_color.h"
#include "draw_color.h"
#include "val_mod.h"
#include "forms_split.h"

#include "event_list.h"

#include "widget_menu_screen.h"

#include "widget_screen_switch.h"
#include "widget_selectable_list.h"

#include "widget_metric_list_item.h"
#include "widget_route_list_item.h"
#include "widget_dlc_dump.h"
#include "widget_settings_title.h"
#include "widget_time_settings.h"
#include "widget_odo_settings.h"
#include "widget_br_settings.h"

#include "honda_dlc_units.h"

extern font_t font_5x7;

static const tf_cfg_t screen_title = {
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

enum {
    MENU_METRICS,
    MENU_ROUTES,
    MENU_DUMP,
    MENU_SETTINGS
};

struct menu {
    ui_node_desc_t * screen_list;
    uint8_t screen_num;
    uint8_t by_idx;
};

static const struct menu menu_list[] = {
    [MENU_METRICS] = {
        .screen_num = 2,
        .screen_list = (ui_node_desc_t []){
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
        }
    },
    [MENU_ROUTES] = {
        .by_idx = 1,
        .screen_num = 3,
        .screen_list = (ui_node_desc_t []){
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
        }
    },
    [MENU_DUMP] = {
        .by_idx = 1,
        .screen_num = 3,
        .screen_list = &(ui_node_desc_t) {
            .widget = &widget_dlc_dump
        }
    },
    [MENU_SETTINGS] = {
        .screen_num = 2,
        .screen_list = (ui_node_desc_t []){
            {
                .widget = &widget_selectable_list,
                .cfg = &(widget_selectable_list_cfg_t) {
                    .num = 3,
                    .margin = { .x = 4, .y = 4 },
                    .ui_node = (ui_node_desc_t[]) {
                        {
                            .widget = &widget_settings_title,
                            .cfg = &(widget_settings_title_cfg_t){
                                .title_list = (const char * []) {
                                    "TIME",
                                    "DATE",
                                    "TIME ZONE",
                                },
                                .screen = &(ui_node_desc_t){
                                    .widget = &widget_time_settings,
                                },
                            }
                        }
                    }
                }
            },
            {
                .widget = &widget_selectable_list,
                .cfg = &(widget_selectable_list_cfg_t) {
                    .num = 2,
                    .different_nodes = 1,
                    .margin = { .x = 4, .y = 4 },
                    .ui_node = (ui_node_desc_t[]) {
                        {
                            .widget = &widget_settings_title,
                            .cfg = &(widget_settings_title_cfg_t){
                                .different_nodes = 1,
                                .title_list = (const char * []) {
                                    "BRIGHTNESS",
                                },
                                .screen = &(ui_node_desc_t){
                                    .widget = &widget_br_settings,
                                },
                            }
                        },
                        {
                            .widget = &widget_settings_title,
                            .cfg = &(widget_settings_title_cfg_t){
                                .different_nodes = 1,
                                .title_list = (const char * []) {
                                    "ODO SHIFT",
                                },
                                .screen = &(ui_node_desc_t){
                                    .widget = &widget_odo_settings,
                                },
                            }
                        }
                    }
                }

            }
        }
    }

};

static const label_color_t title = {
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

static const label_color_t local_title[] = {
    [MENU_METRICS] =    { .color = COLOR(0x130e06), .l = { .xy = { .x = -3 }, .len = 8, .text_list = (const char * []){ "REAL", "BOOL" }, .t = LP_T_LIDX } },
    [MENU_ROUTES] =     { .color = COLOR(0x130e06), .l = { .xy = { .x = -3 }, .len = 8, .text_list = (const char * []){ "ACTUAL", "TRIP", "REFILL" }, .t = LP_T_LIDX } },
    [MENU_DUMP] =       { .color = COLOR(0x130e06), .l = { .xy = { .x = -3 }, .len = 8, .to_str = (const char * (*)(unsigned))honda_dlc_unit_name, .t = LP_T_FIDX } },
    [MENU_SETTINGS] =   { .color = COLOR(0x130e06), .l = { .xy = { .x = -3 }, .len = 8, .text_list = (const char * []){ "DATETIME", "ODO", "ANALOG" }, .t = LP_T_LIDX } },
};

static const label_list_t title_selector = {
    .wrap_list = (label_color_t []) {
        { .color = 0, .l = { .xy = { .x =  0 }, .len = 1, .text_list = (const char *[]){ "[", 0 }, .t = LP_T_LIDX } },
        { .color = 0, .l = { .xy = { .x = 11 }, .len = 1, .text_list = (const char *[]){ "]", 0 }, .t = LP_T_LIDX } },
        { .color = 0, .l = { .xy = { .x = -12 }, .len = 1, .text_list = (const char *[]){ 0, "[" }, .t = LP_T_LIDX } },
        { .color = 0, .l = { .xy = { .x = -1 }, .len = 1, .text_list = (const char *[]){ 0, "]" }, .t = LP_T_LIDX } },
    },
    .count = 4
};

// static const lcd_color_t bg = COLOR(0x4585E1);
static const lcd_color_t bg = COLOR(0xf08400);

static void select_update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    label_color(&ctx->title_ctx, &local_title[el->idx], bg, ctx->selector, 0, 0);
    // label_color_list(&ctx->title_ctx, &title_selector, COLOR(0x4585E1), el->active, 0, 0);
    label_color_list(&ctx->title_ctx, &title_selector, bg, el->active, 0, 0);
}

static void redraw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    select_update(el);

    ui_element_t * item;
    if (menu_list[el->idx].by_idx) {
        item = ui_tree_add(el, menu_list[el->idx].screen_list, ctx->selector);
    } else {
        item = ui_tree_add(el, &menu_list[el->idx].screen_list[ctx->selector], 0);
    }

    form_cut(&item->f, DIMENSION_Y, EDGE_U, ctx->title_form.s.h);

    ui_tree_element_draw(item);
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

    label_color(&ctx->title_ctx, &title, bg, el->idx, 0, 0);

    redraw(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    if (event == EVENT_BTN_DOWN) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_ADD, 1, 0, menu_list[el->idx].screen_num - 1, 1);
        change_cild(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        val_mod_unsigned(&ctx->selector, VAL_SIZE_32, MOD_OP_SUB, 1, 0, menu_list[el->idx].screen_num - 1, 1);
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

static const widget_desc_t widget_menu_screen = {
    .draw = draw,
    .select = select_update,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};

const ui_node_desc_t node_widget_menu_screen = {
    .widget = &widget_screen_switch,
    .cfg = &(widget_screen_switch_cfg_t){
        .screens_num = 4,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &widget_menu_screen,
            }
        }
    }
};
