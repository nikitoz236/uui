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
    tf_ctx_t tf;
    uv_t uv;
} ctx_t;

extern const font_t font_5x7;

static const tf_cfg_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
    },
    .limit_char = { .y = 1 },
    .padding = { .x = 2, .y = 2 },
    .a = ALIGN_LIC,
};

static const lcd_color_t bg[] = { 0x111111, 0x113222 };

static const char * metric_str_bool(unsigned idx)
{
    unsigned val = metric_bool_get_val(idx);
    if (val) {
        return "ACTIVE";
    }
    return "inactive";
}

static const char * raw_hex(unsigned x)
{
    char * str = str_val_buf_get();
    hex_to_str(&x, str, 2);
    return str;
}

static void metric_vt(val_text_t * vt, unsigned idx)
{
    unsigned point = 0;
    dec_factor_t factor = 0;
    metric_var_get_factor_point(idx, &factor, &point);
    vt->f = factor;
    vt->p = point;
    vt->zr = 1;
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

static void update_uv_real(uv_t * uv, unsigned idx)
{
    uv->val = metric_var_get_real(idx);
    uv->raw = metric_ecu_get_raw(idx);
}

static void update_uv_bool(uv_t * uv, unsigned idx)
{
    uv->val = metric_bool_get_val(idx);
}

static const label_list_t ll_static[] = {
    [METRIC_LIST_VAL] = { .count = ARRAY_SIZE(label_static_real), .wrap_list = label_static_real },
    [METRIC_LIST_BOOL] = { .count = 1, .wrap_list = &label_static_bool }
};

static const label_list_t ll_value[] = {
    [METRIC_LIST_VAL] = { .count = ARRAY_SIZE(label_val), .wrap_list = label_val, .ctx_update = (void(*)(void * ctx, unsigned x))update_uv_real },
    [METRIC_LIST_BOOL] = {
        .ctx_update = (void(*)(void * ctx, unsigned x))update_uv_bool,
        .count = 1,
        .wrap_list = (const lp_color_t []) {
            { .color = 0x52AD6F, .l = { .len = 1, .rep = { .vs = VAL_SIZE_32 }, .t = LP_V, .ofs = offsetof(uv_t, val), .xy = { .x = 18 + 9 }, } },
        },
    },
};

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

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_metric_list_item_cfg_t * cfg = (widget_metric_list_item_cfg_t *)el->ui_node->cfg;
    uv_t uv = {};
    lp_color(&ctx->tf, bg[el->active], &ll_value[cfg->type], el->idx, &uv, &ctx->uv);
}

static void select_update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    widget_metric_list_item_cfg_t * cfg = (widget_metric_list_item_cfg_t *)el->ui_node->cfg;
    draw_color_form(&el->f, bg[el->active]);
    lp_color(&ctx->tf, bg[el->active], &ll_static[cfg->type], el->idx, 0, 0);
    lp_color(&ctx->tf, bg[el->active], &ll_value[cfg->type], el->idx, &ctx->uv, 0);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    if (tf_ctx_calc(&ctx->tf, &el->f, &tf)) {
        select_update(el);
        el->drawed = 1;
    }
}

const widget_desc_t widget_metric_list_item = {
    .draw = draw,
    .update = update,
    .select = select_update,
    .ctx_size = sizeof(ctx_t)
};
