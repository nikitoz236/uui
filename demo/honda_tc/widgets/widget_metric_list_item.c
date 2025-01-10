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
    .limit_char = { .x = TEXT_LEN("metric name 0123456 units 0000") },
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
    int val = metric_var_get_raw(idx);
    hex_to_str(&val, str, 2);
    return str;
}

static const char * metric_str_real_val(unsigned idx)
{
    return "12345678";
    // char * str = 
    // val_text_to_str(&metric_ecu_get_real(idx), &);
}

color_scheme_t cs = {
    .fg = 0xfffa75,
    .bg = 0
};

static const text_field_value_t tv[] = {
    [ITEM_NAME]  = { .tfcfg = &tf, .to_str = metric_var_get_name,   .len = METRIC_NAME_MAX_LEN, .pos = { .x = 0 }, },
    [ITEM_UNIT]  = { .tfcfg = &tf, .to_str = metric_var_get_unit,   .len = METRIC_UNIT_MAX_LEN, .pos = { .x = METRIC_NAME_MAX_LEN + 1 + VAL_LEN + 1 }, },
    [ITEM_RAW]   = { .tfcfg = &tf, .to_str = metric_str_raw_val,    .len = 4,                   .pos = { .x = METRIC_NAME_MAX_LEN + 1 + VAL_LEN + 1 + METRIC_UNIT_MAX_LEN + 1 } },
    [ITEM_REAL] = { .tfcfg = &tf, .to_str = metric_str_real_val,   .len = 8,                   .pos = { .x = METRIC_NAME_MAX_LEN + 1 } },
};

static void calc(ui_element_t * el)
{
    el->f.s = text_field_size_px(&tf);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->pos = align_form_pos(&el->f, lcd_text_size_px(tf.fcfg, tf.limit_char), tf.a, tf.padding);;
}

static void update(ui_element_t * el)
{

}

void update_tf(const text_field_value_t * tf, xy_t pos, unsigned idx)
{
    const char * str = tf->to_str(idx);
    lcd_color_text_raw_print(str, tf->tfcfg->fcfg, &cs, &pos, &tf->tfcfg->limit_char, &tf->pos, tf->len);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->real = metric_ecu_get_real(el->idx);
    update_tf(&tv[ITEM_NAME], ctx->pos, el->idx);
    update_tf(&tv[ITEM_UNIT], ctx->pos, el->idx);
    update_tf(&tv[ITEM_REAL], ctx->pos, el->idx);
    update_tf(&tv[ITEM_RAW], ctx->pos, el->idx);



//     __widget_metric_list_item_cfg_t * cfg = (__widget_metric_list_item_cfg_t *)el->ui_node->cfg;
//     __widget_metric_list_item_ctx_t * ctx = (__widget_metric_list_item_ctx_t *)el->ctx;

//     color_scheme_t cs = {
//         .fg = cfg->color_text,
//         .bg = cfg->color_bg
//     };

//     unsigned m_id = el->idx;

//     unsigned pos_val = METRIC_NAME_MAX_LEN + 1;
//     unsigned pos_unit = pos_val + 10;
//     unsigned pos_raw = pos_unit + METRIC_UNIT_MAX_LEN + 1;

//     lcd_color_text_raw_print(metric_var_get_name(m_id), )

//     lcd_text_color_print(metric_var_get_name(m_id), &el->f.p, &ctx->text_cfg, &cs, 0, 0, METRIC_NAME_MAX_LEN + 1);

//     cs.fg = cfg->color_unit;
//     lcd_text_color_print(metric_var_get_unit(m_id), &el->f.p, &ctx->text_cfg, &cs, pos_unit, 0, METRIC_UNIT_MAX_LEN + 1);

//     cs.fg = cfg->color_value;
//     lcd_text_color_print("0123456", &el->f.p, &ctx->text_cfg, &cs, pos_val, 0, 8);
//     cs.fg = cfg->color_raw;
//     lcd_text_color_print("55AA", &el->f.p, &ctx->text_cfg, &cs, pos_raw, 0, 4);


}

const widget_desc_t __widget_metric_list_item = {
    .calc = calc,
    .draw = draw,
    .update = update,
    .extend = extend,
    .ctx_size = sizeof(ctx_t)
};
