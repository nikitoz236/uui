#include "widget_metric_list_item.h"
#include "str_val.h"
#include "forms_align.h"
#include "lcd_text_color.h"
#include "text_field.h"
#include "str_val_buf.h"

typedef struct {
    xy_t pos;
    int real;
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
    ITEM_UNIT
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
    return "12345678";
    // char * str = 
    // val_text_to_str(&metric_ecu_get_real(idx), &);
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

static const item_tf_t items[] = {
    [ITEM_NAME]  = { .cs = &cst, .tv = { .tfcfg = &tf, .to_str = metric_var_get_name,   .len = METRIC_NAME_MAX_LEN, .pos = { .x = 0 }, } },
    [ITEM_UNIT]  = { .cs = &cst, .tv = { .tfcfg = &tf, .to_str = metric_var_get_unit,   .len = METRIC_UNIT_MAX_LEN, .pos = { .x = METRIC_NAME_MAX_LEN + 1 + VAL_LEN + 1 }, } },
    [ITEM_RAW]   = { .cs = &csv, .tv = { .tfcfg = &tf, .to_str = metric_str_raw_val,    .len = 4,                   .pos = { .x = METRIC_NAME_MAX_LEN + 1 + VAL_LEN + 1 + METRIC_UNIT_MAX_LEN + 1 } } },
    [ITEM_REAL]  = { .cs = &csv, .tv = { .tfcfg = &tf, .to_str = metric_str_real_val,   .len = 8,                   .pos = { .x = METRIC_NAME_MAX_LEN + 1 } } },
};

static void calc(ui_element_t * el)
{
    el->f.s = text_field_size_px(&tf);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->pos = align_form_pos(&el->f, lcd_text_size_px(tf.fcfg, tf.limit_char), tf.a, tf.padding);
    printf("widget_metric_list_item extend item %d size %d %d, text pos %d %d\n", el->idx, el->f.s.w, el->f.s.h, ctx->pos.x, ctx->pos.y);
}

void update_tf(const item_tf_t * item, xy_t pos, unsigned idx)
{
    const char * str = item->tv.to_str(idx);
    lcd_color_text_raw_print(str, item->tv.tfcfg->fcfg, item->cs, &pos, &item->tv.tfcfg->limit_char, &item->tv.pos, item->tv.len);
}

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    int real = metric_var_get_real(el->idx);
    if (ctx->real != real) {
        ctx->real = real;
        update_tf(&items[ITEM_REAL], ctx->pos, el->idx);
        update_tf(&items[ITEM_RAW], ctx->pos, el->idx);
    }
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->real = metric_ecu_get_real(el->idx);
    update_tf(&items[ITEM_NAME], ctx->pos, el->idx);
    update_tf(&items[ITEM_UNIT], ctx->pos, el->idx);
    update_tf(&items[ITEM_REAL], ctx->pos, el->idx);
    update_tf(&items[ITEM_RAW], ctx->pos, el->idx);
}

const widget_desc_t __widget_metric_list_item = {
    .calc = calc,
    .draw = draw,
    .update = update,
    .extend = extend,
    .ctx_size = sizeof(ctx_t)
};
