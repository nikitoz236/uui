#include "widget_route_list_item.h"

#include "text_field.h"
#include "draw_color.h"
#include "array_size.h"
#include <stddef.h>

#include "routes.h"
#include "date_time.h"
#include "time_zone.h"

#include "event_list.h"
#include "text_label_color.h"

/*
    достаточно типичная ситуация - горизонтальный элемент
    как выглядит работа с ним

    на стадии calc вычисляется размер формы - дальше форму такто надо сохранить если это например заголовок чтобы можно было ее закрасить на стадии draw

    интересная мысль 1 - выравниания. можно сделать отрицательные значения координат для выравнивания по правому / нижнему краю

    что делаем с листабельным текстом ?

*/

extern const font_t font_5x7;

const text_field_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .y = 4 },
    },
    .a = ALIGN_CC,
    .padding = { .x = 8, .y = 8 },
    .limit_char = { .x = 50, .y = 4 },      // ну вот он же может расширятся, и интерфейс может расширятся, есть поля привязаные к правому краю, хм
};

const lcd_color_t bg[] = { 0x111111, 0x113222 };

struct route_time {
    unsigned h;
    uint8_t s;
    uint8_t m;
};

struct route_since {
    time_t t;
    date_t d;
};

struct updated_val {
    unsigned rv[ROUTE_VALUE_NUM];
    struct route_time time;
    struct route_since since;
};

typedef struct {
    xy_t tp;
    struct updated_val uv;
    uint8_t restart_engaged;
} ctx_t;

struct lscolor {
    lcd_color_t color;
    label_static_t l;
};

struct lvcolor {
    lcd_color_t color;
    label_value_t l;
};

const struct lscolor label_static[] = {
    { .color = 0x882222, .l = { .xy = { .x = 0,  .y = 0 }, .to_str = route_name    } },

    { .color = 0x555555, .l = { .xy = { .x = 30, .y = 0 }, .text = "time:",        } },
    { .color = 0x555555, .l = { .xy = { .x = 44, .y = 0 }, .text = ":",            } },
    { .color = 0x555555, .l = { .xy = { .x = 47, .y = 0 }, .text = ":",            } },
    { .color = 0x555555, .l = { .xy = { .x = 0,  .y = 1 }, .text = "start:",       } },
    { .color = 0x96A41d, .l = { .xy = { .x = 21, .y = 1 }, .text = ":",            } },
    { .color = 0x96A41d, .l = { .xy = { .x = 24, .y = 1 }, .text = ":",            } },

    { .color = 0x555555, .l = { .xy = { .x = 30, .y = 2 }, .text = "dist:",        } },
    { .color = 0x555555, .l = { .xy = { .x = 48, .y = 2 }, .text = "km",           } },

    { .color = 0x555555, .l = { .xy = { .x = 0,  .y = 2 }, .text = "avg speed:",   } },
    { .color = 0x555555, .l = { .xy = { .x = 18, .y = 2 }, .text = "km/h",         } },

    { .color = 0x555555, .l = { .xy = { .x = 30, .y = 3 }, .text = "fuel:",        } },
    { .color = 0x555555, .l = { .xy = { .x = 49, .y = 3 }, .text = "L",            } },

    { .color = 0x555555, .l = { .xy = { .x = 0,  .y = 3 }, .text = "cons:",        } },
    { .color = 0x555555, .l = { .xy = { .x = 12, .y = 3 }, .text = "L/h",          } },
    { .color = 0x555555, .l = { .xy = { .x = 22, .y = 3 }, .text = "L/100km",      } },

    { .color = 0x555555, .l = { .xy = { .x = 31, .y = 1 }, .text = "odo:",         } },
    { .color = 0x555555, .l = { .xy = { .x = 48, .y = 1 }, .text = "km",           } },
};

const struct lscolor label_restart[] = {
    { .color = 0xFF0000, .l = { .xy = { .x = 14, .y = 0 }, .text = 0,               .len = 13 } },
    { .color = 0xFF0000, .l = { .xy = { .x = 14, .y = 0 }, .text = "OK to RESTART", .len = 13 } }
};

const label_value_t labels_route_time[] = {
    { .t = LV, .xy = { .x = 37, .y = 0 }, .rep = { .vs = VAL_SIZE_32 }, .len = 7, .vt = { .zl = 0 },     .ofs = offsetof(struct route_time, h) },
    { .t = LV, .xy = { .x = 45, .y = 0 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(struct route_time, m) },
    { .t = LV, .xy = { .x = 48, .y = 0 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(struct route_time, s) },
};

const label_value_t labels_route_since[] = {
    { .t = LV, .xy = { .x = 7,  .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 0 },     .ofs = offsetof(struct route_since, d.d) },
    { .t = LF, .xy = { .x = 10, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 3, .to_str = month_name,  .ofs = offsetof(struct route_since, d.m) },
    { .t = LV, .xy = { .x = 14, .y = 1 }, .rep = { .vs = VAL_SIZE_16 }, .len = 4, .vt = { .zl = 0 },     .ofs = offsetof(struct route_since, d.y) },
    { .t = LV, .xy = { .x = 19, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(struct route_since, t.h) },
    { .t = LV, .xy = { .x = 22, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(struct route_since, t.m) },
    { .t = LV, .xy = { .x = 25, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(struct route_since, t.s) },
};

static void ctx_update_time(struct route_time * rt, unsigned time_s)
{
    rt->s = time_s % 60;
    time_s /= 60;
    rt->m = time_s % 60;
    rt->h = time_s / 60;
}

static void ctx_update_since(struct route_since * rs, unsigned since_s)
{
    date_from_s(&rs->d, since_s);
    time_from_s(&rs->t, since_s);
}

const struct lvcolor labels_vals[] = {
    [ROUTE_VALUE_DIST] =        { .color = 0x96A41d, .l = { .t = LV, .xy = { .x = 36, .y = 2 }, .len = 11,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_DIST]) } },
    [ROUTE_VALUE_FUEL] =        { .color = 0x96A4Ad, .l = { .t = LV, .xy = { .x = 37, .y = 3 }, .len = 11,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_FUEL]) } },
    [ROUTE_VALUE_SINCE_ODO] =   { .color = 0x96A41d, .l = { .t = LV, .xy = { .x = 37, .y = 1 }, .len = 10,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_SINCE_ODO]) } },
    [ROUTE_VALUE_AVG_SPEED] =   { .color = 0x96A41d, .l = { .t = LV, .xy = { .x = 11, .y = 2 }, .len = 6,   .vt = { .f = X1000, .p = 2, .zr = 0}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_AVG_SPEED]) } },
    [ROUTE_VALUE_CONS_DIST] =   { .color = 0x12fa44, .l = { .t = LV, .xy = { .x = 16, .y = 3 }, .len = 5,   .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_CONS_DIST]) } },
    [ROUTE_VALUE_CONS_TIME] =   { .color = 0x12fa44, .l = { .t = LV, .xy = { .x = 6,  .y = 3 }, .len = 5,   .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_CONS_TIME]) } },
    [ROUTE_VALUE_SINCE_TIME] =  { .color = 0xE6A7bd, .l = { .t = LS, .sofs = offsetof(struct updated_val, since), .rep = { .vs = VAL_SIZE_32}, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_SINCE_TIME]), .sl = &(const struct sub_label_list){ .ctx_update = (void(*)(void * ctx, unsigned x))ctx_update_since, .list = labels_route_since, .count = 6 } } },
    [ROUTE_VALUE_TIME] =        { .color = 0x96A41d, .l = { .t = LS, .sofs = offsetof(struct updated_val, time),  .rep = { .vs = VAL_SIZE_32}, .ofs = offsetof(struct updated_val, rv[ROUTE_VALUE_TIME]),       .sl = &(const struct sub_label_list){ .ctx_update = (void(*)(void * ctx, unsigned x))ctx_update_time,  .list = labels_route_time,  .count = 3 } } },
};

/*
vscode показывает символы начиная с 1, графика с 0
--------------------------------------------------
JOURNEY     RESTART? (OK)       time: 124562:34:23
start: 21 DEC 2024 13:34:23   dist: 1245673.343 km
avg speed: 253.45 km/h         fuel: 2334567.233 L
cons: 37.56 L/h 37.56 L/100km   odo: 345674.324 km
--------------------------------------------------
*/

static void calc(ui_element_t * el)
{
    el->f.s = text_field_size_px(&tf);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->tp = text_field_text_pos(&el->f, &tf);
}

static void ctx_update_vals(struct updated_val * uv, route_t r)
{
    for (route_value_t t = 0; t < ROUTE_VALUE_NUM; t++) {
        uv->rv[t] = route_get_value(r, t);
    }
}

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    route_t r = el->idx;
    color_scheme_t cs = {
        .bg = bg[el->active],
    };

    tf_ctx_t tf_ctx = {
        .tfcfg = &tf,
        .xy = ctx->tp,
    };

    struct updated_val uv;
    ctx_update_vals(&uv, r);
    for (unsigned i = 0; i < ARRAY_SIZE(labels_vals); i++) {
        cs.fg = labels_vals[i].color;
        label_value_print(&tf_ctx, &labels_vals[i].l, &cs, &ctx->uv, &uv);
    }
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->restart_engaged = 0;
    route_t r = el->idx;

    color_scheme_t cs = {
        .bg = bg[el->active],
    };

    tf_ctx_t tf_ctx = {
        .tfcfg = &tf,
        .xy = ctx->tp,
    };

    draw_color_form(&el->f, cs.bg);
    for (unsigned i = 0; i < ARRAY_SIZE(label_static); i++) {
        cs.fg = label_static[i].color;
        label_static_print(&tf_ctx, &label_static[i].l, &cs, r);
    }

    ctx_update_vals(&ctx->uv, r);
    for (unsigned i = 0; i < ARRAY_SIZE(labels_vals); i++) {
        cs.fg = labels_vals[i].color;
        label_value_print(&tf_ctx, &labels_vals[i].l, &cs, 0, &ctx->uv);
    }
}

static void update_restart_engaged(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    color_scheme_t cs = {
        .bg = bg[1],
        .fg = label_restart[ctx->restart_engaged].color
    };

    tf_ctx_t tf_ctx = {
        .tfcfg = &tf,
        .xy = ctx->tp,
    };

    label_static_print(&tf_ctx, &label_restart[ctx->restart_engaged].l, &cs, 0);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    if (event == EVENT_BTN_OK) {
        if (ctx->restart_engaged) {
            route_reset(el->idx);
            ctx->restart_engaged = 0;
            update_restart_engaged(el);
            update(el);
        } else {
            if ((el->idx == ROUTE_TYPE_TRIP) || (el->idx == ROUTE_TYPE_TOTAL) || (el->idx == ROUTE_TYPE_DAY)) {
                return 1;
            }
            ctx->restart_engaged = 1;
            update_restart_engaged(el);
        }
        return 1;
    }
    return 0;
}

const widget_desc_t widget_route_list_item = {
    .calc = calc,
    .extend = extend,
    .update = update,
    .draw = draw,
    .select = draw,
    .process_event = process,
    .ctx_size = sizeof(ctx_t),
};
