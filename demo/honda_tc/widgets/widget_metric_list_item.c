#include "widget_metric_list_item.h"
#include "str_val.h"
#include "forms_align.h"
#include "lcd_text_color.h"
#include "text_field.h"
#include "str_val_buf.h"

typedef struct {
    xy_t pos;
    int val;
} ctx_t;

extern const font_t font_5x7;

static const text_field_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
    },
    .limit_char = { .x = TEXT_LEN("metric name      12345678 units 0000") },
    .padding = { .x = 2, .y = 2 },
    .a = ALIGN_LIC,
};

enum {
    ITEM_NAME,
    ITEM_REAL,
    ITEM_RAW,
    ITEM_UNIT,
    ITEM_BOOL = ITEM_REAL,
};

#define VAL_LEN 8

static const char * metric_str_raw_val(unsigned idx)
{
    char * str = str_val_buf_get();
    unsigned val = metric_var_get_raw(idx);
    hex_to_str(&val, str, 2);
    return str;
}

static const char * metric_str_real_val(unsigned idx)
{
    char * str = str_val_buf_get();
    int val = metric_var_get_real(idx);
    val_text_t vt = {
        .f = metric_var_get_factor(idx),
        .p = 2,
        .s = 1,
        .zl = 0,
        .zr = 0,
        .t = DEC,
        .vs = VAL_SIZE_32,
        .l = VAL_LEN
    };
    val_text_to_str(str, &val, &vt);
    return str;
}

static const char * metric_str_bool(unsigned idx)
{
    unsigned val = metric_bool_get_val(idx);
    if (val) {
        return "ACTIVE";
    }
    return "inactive";
}

color_scheme_t cst = {
    .fg = 0xfffa75,
    .bg = 0
};

color_scheme_t csv = {
    .fg = 0xff2a35,
    .bg = 0
};

typedef struct {
    text_field_value_t tv;
    color_scheme_t * cs;
} item_tf_t;

static const item_tf_t items_real[] = {
    [ITEM_NAME]  = { .cs = &cst, .tv = { .tfcfg = &tf, .to_str = metric_var_get_name,   .len = METRIC_NAME_MAX_LEN, .pos = { .x = 0 }, } },
    [ITEM_UNIT]  = { .cs = &cst, .tv = { .tfcfg = &tf, .to_str = metric_var_get_unit,   .len = METRIC_UNIT_MAX_LEN, .pos = { .x = METRIC_NAME_MAX_LEN + 1 + VAL_LEN + 1 }, } },
    [ITEM_RAW]   = { .cs = &csv, .tv = { .tfcfg = &tf, .to_str = metric_str_raw_val,    .len = 4,                   .pos = { .x = METRIC_NAME_MAX_LEN + 1 + VAL_LEN + 1 + METRIC_UNIT_MAX_LEN + 1 } } },
    [ITEM_REAL]  = { .cs = &csv, .tv = { .tfcfg = &tf, .to_str = metric_str_real_val,   .len = 8,                   .pos = { .x = METRIC_NAME_MAX_LEN + 1 } } },
};

static const item_tf_t items_bool[] = {
    [ITEM_NAME]  = { .cs = &cst, .tv = { .tfcfg = &tf, .to_str = metric_bool_get_name,  .len = METRIC_NAME_MAX_LEN, .pos = { .x = 0 }, } },
    [ITEM_BOOL]  = { .cs = &csv, .tv = { .tfcfg = &tf, .to_str = metric_str_bool,       .len = TEXT_LEN("inactive"),.pos = { .x = METRIC_NAME_MAX_LEN + 1 } } },
};

static void calc(ui_element_t * el)
{
    el->f.s = text_field_size_px(&tf);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->pos = align_form_pos(&el->f, lcd_text_size_px(tf.fcfg, tf.limit_char), tf.a, tf.padding);
    // printf("widget_metric_list_item extend item %d size %d %d, text pos %d %d\n", el->idx, el->f.s.w, el->f.s.h, ctx->pos.x, ctx->pos.y);
}

void update_tf(const item_tf_t * item, xy_t pos, unsigned idx)
{
    const char * str = item->tv.to_str(idx);
    lcd_color_text_raw_print(str, item->tv.tfcfg->fcfg, item->cs, &pos, &item->tv.tfcfg->limit_char, &item->tv.pos, item->tv.len);
}

static void update_real(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    int real = metric_var_get_real(el->idx);
    if (ctx->val != real) {
        ctx->val = real;
        update_tf(&items_real[ITEM_REAL], ctx->pos, el->idx);
        update_tf(&items_real[ITEM_RAW], ctx->pos, el->idx);
    }
}

static void update_bool(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    int val = metric_bool_get_val(el->idx);
    if (ctx->val != val) {
        ctx->val = val;
        update_tf(&items_bool[ITEM_NAME], ctx->pos, el->idx);
        update_tf(&items_bool[ITEM_BOOL], ctx->pos, el->idx);
    }
}

static void draw_real(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->val = metric_ecu_get_real(el->idx);
    update_tf(&items_real[ITEM_NAME], ctx->pos, el->idx);
    update_tf(&items_real[ITEM_UNIT], ctx->pos, el->idx);
    update_tf(&items_real[ITEM_REAL], ctx->pos, el->idx);
    update_tf(&items_real[ITEM_RAW], ctx->pos, el->idx);
}

static void draw_bool(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->val = metric_ecu_get_bool(el->idx);
    update_tf(&items_bool[ITEM_NAME], ctx->pos, el->idx);
    update_tf(&items_bool[ITEM_BOOL], ctx->pos, el->idx);
}

const widget_desc_t __widget_metric_list_item_real = {
    .calc = calc,
    .draw = draw_real,
    .update = update_real,
    .extend = extend,
    .ctx_size = sizeof(ctx_t)
};

const widget_desc_t __widget_metric_list_item_bool = {
    .calc = calc,
    .draw = draw_bool,
    .update = update_bool,
    .extend = extend,
    .ctx_size = sizeof(ctx_t)
};
