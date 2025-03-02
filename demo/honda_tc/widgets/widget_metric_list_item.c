#include "widget_metric_list_item.h"
#include "str_val.h"
#include "forms_align.h"
#include "lcd_text_color.h"
#include "text_field.h"
#include "text_label_color.h"
#include "array_size.h"

#include "str_val_buf.h"
#include "str_val.h"
#include "draw_color.h"

typedef struct {
    int val;
    unsigned raw;
} uv_t;

typedef struct {
    xy_t pos;
    uv_t uv;
} ctx_t;

extern const font_t font_5x7;

static const text_field_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
    },
    .limit_char = { .x = 50 },
    .padding = { .x = 2, .y = 2 },
    .a = ALIGN_LIC,
};

const lcd_color_t bg[] = { 0x111111, 0x113222 };

static const char * metric_str_bool(unsigned idx)
{
    unsigned val = metric_bool_get_val(idx);
    if (val) {
        return "ACTIVE";
    }
    return "inactive";
}


const char * raw_hex(unsigned x)
{
    char * str = str_val_buf_get();
    hex_to_str(&x, str, 2);
    return str;
}

void metric_vt(val_text_t * vt, unsigned idx)
{
    unsigned point = 0;
    dec_factor_t factor = 0;
    metric_var_get_factor_point(idx, &factor, &point);
    vt->f = factor;
    vt->p = point;
    vt->zr = 0;
    vt->zl = 0;
};

/*
=================
0                 18            32        42
metric name       -12345678901  units     0000
=======================================

*/

static const lp_color_t label_static_real[] = {
    { .color = 0x524D3F, .l = { .to_str = metric_var_get_name, .t = LP_T_FIDX, .xy = { .x = 0 }, } },
    { .color = 0x1D5F90, .l = { .to_str = metric_var_get_unit, .t = LP_T_FIDX, .xy = { .x = 32 }, } },
};

static const lp_color_t label_static_bool = {
      .color = 0x1D5F90, .l = { .to_str = metric_bool_get_name, .t = LP_T_FIDX, .xy = { .x = 0 }, }
};

static const lp_color_t label_val[] = {
    { .color = 0x52AD6F, .l = { .len = 10, .rep = { .s = 1, .vs = VAL_SIZE_32 }, .t = LVFI, .vt_by_idx = metric_vt, .ofs = offsetof(uv_t, val), .xy = { .x = 18 }, } },
    { .color = 0xAB4422, .l = { .len = 4,  .rep = { .vs = VAL_SIZE_32 }, .t = LF, .to_str = raw_hex, .ofs = offsetof(uv_t, raw), .xy = { .x = 42 }, } },
};

void update_uv(uv_t * uv, unsigned idx)
{
    uv->val = metric_var_get_real(idx);
    uv->raw = metric_ecu_get_raw(idx);
}

const label_list_t ll_static_real = { .count = ARRAY_SIZE(label_static_real), .wrap_list = label_static_real };
const label_list_t ll_val_real = { .count = ARRAY_SIZE(label_val), .wrap_list = label_val, .ctx_update = (void(*)(void * ctx, unsigned x))update_uv };

static void calc(ui_element_t * el)
{
    el->f.s = text_field_size_px(&tf);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->pos = align_form_pos(&el->f, lcd_text_size_px(tf.fcfg, tf.limit_char), tf.a, tf.padding);
}

/*
    итак

    у нас есть здесь просто текстовое поле, в котором как минимум есть заголовок, который зависит от индекса
        для значений в нем печатаются
            само значение которое точно получается в контекст из индекса
                но печатать его непонятно как, у каждого свое представление, знак, фактор, количество символов после запятой итд, тоже по индексу становится понятно
            единица измерения которая зависит от индекса
            сырое значение из контекста
        для булевых значений 

*/

static void update_real(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    tf_ctx_t tf_ctx = {
        .tfcfg = &tf,
        .xy = ctx->pos,
    };

    uv_t uv = {};
    lp_color(&tf_ctx, bg[el->active], &ll_val_real, el->idx, &uv, &ctx->uv);
}

static void update_bool(ui_element_t * el)
{

}

static void draw_real(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    tf_ctx_t tf_ctx = {
        .tfcfg = &tf,
        .xy = ctx->pos,
    };
    draw_color_form(&el->f, bg[el->active]);
    lp_color(&tf_ctx, bg[el->active], &ll_static_real, el->idx, 0, 0);
    lp_color(&tf_ctx, bg[el->active], &ll_val_real, el->idx, &ctx->uv, 0);
}

static void draw_bool(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    color_scheme_t cs = {
        .bg = bg[el->active],
    };

    tf_ctx_t tf_ctx = {
        .tfcfg = &tf,
        .xy = ctx->pos,
    };

    cs.fg = label_static_bool.color;
    lp(&tf_ctx, &label_static_bool.l, &cs, 0, 0, el->idx);

    // ctx->val = metric_ecu_get_bool(el->idx);
}

const widget_desc_t __widget_metric_list_item_real = {
    .calc = calc,
    .draw = draw_real,
    .update = update_real,
    .extend = extend,
    .select = draw_real,
    .ctx_size = sizeof(ctx_t)
};

const widget_desc_t __widget_metric_list_item_bool = {
    .calc = calc,
    .draw = draw_bool,
    // .update = update_bool,
    .extend = extend,
    .select = draw_bool,
    .ctx_size = sizeof(ctx_t)
};
