#include "widget_route_list_item.h"

#include "text_field.h"
#include "draw_color.h"
#include "array_size.h"
#include <stddef.h>

#include "date_time.h"
#include "time_zone.h"

#include "event_list.h"
#include "text_label_color.h"

extern const font_t font_5x7;

const tf_cfg_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .y = 4 },
    },
    .a = ALIGN_CC,
    .padding = { .x = 8, .y = 6 },
    .limit_char = { .y = 4 },      // ну вот он же может расширятся, и интерфейс может расширятся, есть поля привязаные к правому краю, хм
};

const lcd_color_t bg[] = { COLOR(0x111111), COLOR(0x113222) };

typedef struct {
    unsigned h;
    uint8_t s;
    uint8_t m;
} route_time_t;

typedef struct {
    time_t t;
    date_t d;
} route_since_t;

typedef struct {
    unsigned rv[ROUTE_VALUE_NUM];
    route_time_t time;
    route_since_t since;
} uv_t;

typedef struct {
    tf_ctx_t tf;
    uv_t uv;
    uint8_t restart_engaged;
} ctx_t;

static void ctx_update_time(route_time_t * rt, unsigned time_s)
{
    rt->s = time_s % 60;
    time_s /= 60;
    rt->m = time_s % 60;
    rt->h = time_s / 60;
}

static void ctx_update_since(route_since_t * rs, unsigned since_s)
{
    since_s += time_zone_get();
    date_from_s(&rs->d, since_s);
    time_from_s(&rs->t, since_s);
}

static void ctx_update_vals(uv_t * uv, route_t r)
{
    for (route_value_t t = 0; t < ROUTE_VALUE_NUM; t++) {
        uv->rv[t] = route_get_value(r, t);
    }
}

/*
vscode показывает символы начиная с 1, графика с 0
--------------------------------------------------
JOURNEY     RESTART? (OK)       time: 124562:34:23
start: 21 DEC 2024 13:34:23   dist: 1245673.343 km
avg speed: 253.45 km/h         fuel: 2334567.233 L
cons: 37.56 L/h 37.56 L/100km   odo: 345674.324 km
--------------------------------------------------

--------------------------
JOURNEY      RESTART? (OK)
124562:34:23 2334567.233 L
1245673.343 km 253.45 km/h
37.56 L/h    37.56 L/100km
start 21 DEC 2024 13:34:23
             345674.324 km
--------------------------
*/

static const label_color_t label_static[] = {
    { .color = COLOR(0x882222), .l = { .t = LP_T_FIDX, .xy = { .x = 0,  .y = 0 }, .to_str = (const char * (*)(unsigned))route_name } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 30 - 52, .y = 0 }, .text = "time:",        } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 44 - 52, .y = 0 }, .text = ":",            } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 47 - 52, .y = 0 }, .text = ":",            } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 0,       .y = 1 }, .text = "start:",       } },
    { .color = COLOR(0x96A41d), .l = { .t = LP_T, .xy = { .x = 21,      .y = 1 }, .text = ":",            } },
    { .color = COLOR(0x96A41d), .l = { .t = LP_T, .xy = { .x = 24,      .y = 1 }, .text = ":",            } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 30 - 52, .y = 2 }, .text = "dist:",        } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 48 - 52, .y = 2 }, .text = "km",           } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 0,       .y = 2 }, .text = "avg speed:",   } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 18,      .y = 2 }, .text = "km/h",         } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 30 - 52, .y = 3 }, .text = "fuel:",        } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 49 - 52, .y = 3 }, .text = "L",            } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 0,       .y = 3 }, .text = "cons:",        } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 12,      .y = 3 }, .text = "L/h",          } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 22,      .y = 3 }, .text = "L/100km",      } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 31 - 52, .y = 1 }, .text = "odo:",         } },
    { .color = COLOR(0x555555), .l = { .t = LP_T, .xy = { .x = 48 - 52, .y = 1 }, .text = "km",           } },
};

static const label_list_t ll_static = { .count = ARRAY_SIZE(label_static), .wrap_list = label_static };

static const label_list_t ll_restart = {
    .count = 1, .wrap_list = (const label_color_t []){
        { .color = COLOR(0xFF0000), .l = { .t = LP_T_LV, .xy = { .x = 14, .y = 0 }, .text_list = (const char *[]){ 0, "OK to RESTART" }, .len = 13, .rep = { .vs = VAL_SIZE_8 }, .ofs = offsetof(ctx_t, restart_engaged) } }
    }
};

static const label_t labels_route_time[] = {
    { .t = LV, .xy = { .x = 37, .y = 0 }, .rep = { .vs = VAL_SIZE_32 }, .len = 7, .vt = { .zl = 0 },     .ofs = offsetof(route_time_t, h) },
    { .t = LV, .xy = { .x = 45, .y = 0 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(route_time_t, m) },
    { .t = LV, .xy = { .x = 48, .y = 0 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(route_time_t, s) },
};

static const label_t labels_route_since[] = {
    { .t = LV, .xy = { .x = 7,  .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 0 },     .ofs = offsetof(route_since_t, d.d) },
    { .t = LF, .xy = { .x = 10, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 3, .to_str = month_name,  .ofs = offsetof(route_since_t, d.m) },
    { .t = LV, .xy = { .x = 14, .y = 1 }, .rep = { .vs = VAL_SIZE_16 }, .len = 4, .vt = { .zl = 0 },     .ofs = offsetof(route_since_t, d.y) },
    { .t = LV, .xy = { .x = 19, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(route_since_t, t.h) },
    { .t = LV, .xy = { .x = 22, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(route_since_t, t.m) },
    { .t = LV, .xy = { .x = 25, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .ofs = offsetof(route_since_t, t.s) },
};

static const label_color_t labels_vals[] = {
    { .color = COLOR(0x96A41d), .l = { .t = LV, .xy = { .x = 36, .y = 2 }, .len = 11,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_DIST]) } },
    { .color = COLOR(0x96A4Ad), .l = { .t = LV, .xy = { .x = 37, .y = 3 }, .len = 11,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_FUEL]) } },
    { .color = COLOR(0x96A41d), .l = { .t = LV, .xy = { .x = 37, .y = 1 }, .len = 10,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_SINCE_ODO]) } },
    { .color = COLOR(0x96A41d), .l = { .t = LV, .xy = { .x = 11, .y = 2 }, .len = 6,   .vt = { .f = X1000, .p = 2, .zr = 0}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_AVG_SPEED]) } },
    { .color = COLOR(0x12fa44), .l = { .t = LV, .xy = { .x = 16, .y = 3 }, .len = 5,   .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_CONS_DIST]) } },
    { .color = COLOR(0x12fa44), .l = { .t = LV, .xy = { .x = 6,  .y = 3 }, .len = 5,   .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_CONS_TIME]) } },
    { .color = COLOR(0xE6A7bd), .l = { .t = LS, .sofs = offsetof(uv_t, since), .rep = { .vs = VAL_SIZE_32}, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_SINCE_TIME]), .sl = &(const label_list_t){ .ctx_update = (void(*)(void * ctx, unsigned x))ctx_update_since, .list = labels_route_since, .count = 6 } } },
    { .color = COLOR(0x96A41d), .l = { .t = LS, .sofs = offsetof(uv_t, time),  .rep = { .vs = VAL_SIZE_32}, .ofs = offsetof(uv_t, rv[ROUTE_VALUE_TIME]),       .sl = &(const label_list_t){ .ctx_update = (void(*)(void * ctx, unsigned x))ctx_update_time,  .list = labels_route_time,  .count = 3 } } },
};

static const label_list_t ll_vals = { .count = ARRAY_SIZE(labels_vals), .wrap_list = labels_vals, .ctx_update = (void(*)(void * ctx, unsigned x))ctx_update_vals };

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    uv_t uv;
    label_color_list(&ctx->tf, &ll_vals, bg[el->active], el->idx, &uv, &ctx->uv);
}

static void select(ui_element_t * el)
{
    // printf("            widget_route_list_item select, active %d, idx %d\n", el->active, el->idx);
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->restart_engaged = 0;

    draw_color_form(&el->f, bg[el->active]);

    label_color_list(&ctx->tf, &ll_static, bg[el->active], el->idx, &ctx->uv, 0);
    label_color_list(&ctx->tf, &ll_vals, bg[el->active], el->idx, &ctx->uv, 0);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    if (tf_ctx_calc(&ctx->tf, &el->f, &tf)) {
        select(el);
        el->drawed = 1;
    }
}

static void update_restart_engaged(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    label_color_list(&ctx->tf, &ll_restart, bg[el->active], el->idx, ctx, 0);

    // проверяй что файл поменялся прежде чем сохранять маршрут
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
    .update = update,
    .draw = draw,
    .select = select,
    .process_event = process,
    .ctx_size = sizeof(ctx_t),
};
