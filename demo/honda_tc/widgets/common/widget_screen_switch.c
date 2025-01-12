#include "widget_screen_switch.h"
#include "event_list.h"

#include "forms_align.h"
#include "forms_split.h"
#include "draw_color.h"
#include "lcd_text_color.h"

typedef struct {
    form_t title_form;
    uint8_t last_selector;
} ctx_t;

static void calc(ui_element_t * el)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;
    unsigned current_screen = *cfg->selector_ptr;
    ui_node_desc_t * node = &cfg->screens_list[current_screen];
    ui_element_t * item = ui_tree_add(el, node, current_screen);
    ui_tree_element_calc(item);
    el->f.s = item->f.s;
    if (cfg->title_cfg) {
        ctx->title_form.s = text_field_size_px(cfg->title_cfg);
        el->f.s.h += ctx->title_form.s.h;
    }
}

static void extend(ui_element_t * el)
{
    ui_element_t * item = ui_tree_child(el);
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;
    item->f = el->f;
    if (cfg->title_cfg) {
        ctx->title_form.p = el->f.p;
        ctx->title_form.s.w = el->f.s.w;
        form_cut(&item->f, DIMENSION_Y, EDGE_U, ctx->title_form.s.h);
    }
    ui_tree_element_extend(item);
}

static void draw_new_child(ui_element_t * el)
{
    ui_tree_delete_childs(el);
    calc(el);
    extend(el);
    ui_tree_element_draw(el);
}

static void draw(ui_element_t * el)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;
    ctx_t * ctx = (ctx_t *)el->ctx;
    if (cfg->title_cfg) {
        draw_color_form(&ctx->title_form, 0x76ab23);
        xy_t title_pos = align_form_pos(&ctx->title_form, text_field_size_px(cfg->title_cfg), cfg->title_cfg->a, cfg->title_cfg->padding);
        unsigned current_screen = *cfg->selector_ptr;
        lcd_color_text_raw_print(cfg->titles[current_screen], cfg->title_cfg->fcfg, &(color_scheme_t){ .bg = 0x76ab23, .fg = 0xAE349E }, &title_pos, 0, 0, 0);
    }
    ui_element_t * item = ui_tree_child(el);
    ui_tree_element_draw(item);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    __widget_screen_switch_cfg_t * cfg = (__widget_screen_switch_cfg_t *)el->ui_node->cfg;

    ui_element_t * item = ui_tree_child(el);

    uint8_t * current_screen = cfg->selector_ptr;
    if (event == EVENT_BTN_DOWN) {
        (*current_screen)++;
        if (*current_screen == cfg->screens_num) {
            *current_screen = 0;
        }
        draw_new_child(el);
        return 1;
    }
    if (event == EVENT_BTN_UP) {
        if (*current_screen == 0) {
            *current_screen = cfg->screens_num;
        }
        (*current_screen)--;

        draw_new_child(el);
        return 1;
    }
    if (event == EVENT_BTN_OK) {
        ui_tree_element_select(item, 1);
        return 1;
    }
    if (event == EVENT_BTN_LEFT) {
        if (item->active) {
            ui_tree_element_select(item, 0);
            return 1;
        }
    }
    return 0;
}

widget_desc_t __widget_screen_switch = {
    .calc = calc,
    .extend = extend,
    .draw = draw,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
