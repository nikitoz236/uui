#include "widget_route_list_item.h"
#include "lcd_text_color.h"
#include "text_field.h"
#include "draw_color.h"
#include "array_size.h"
#include <stddef.h>

#include "routes.h"
#include "date_time.h"
#include "time_zone.h"
#include "str_utils.h"

#include "event_list.h"
#include "text_label_color.h"

#include <stdio.h>


/*
    достаточно типичная ситуация - горизонтальный элемент
    как выглядит работа с ним

    на стадии calc вычисляется размер формы - дальше форму такто надо сохранить если это например заголовок чтобы можно было ее закрасить на стадии draw

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

/*
    можно coord_t сделать знаковым, тогда можно вектора сдвиги делать итд, выравнивать по правому краю итд


    чем ты тут вообще занимаешься

    я хочу удобный инструментарий для рисования виджета

    а именно

    расположение статичных текстов по координатам.
        тут есть общие свойства
            текстовое поле со шрифтом и начальными координатами
                например шрифт
            координаты знакоместа
            цвет, выделение

    расположение переменнх по координатам, тут сложнее
        общие свойства + цвет может отличаться
        свойства отрисовки значения
        как получить значение

    тут как будто надо 2 структуры ?




    интересная мысль 1 - выравниания. можно сделать отрицательные значения координат для выравнивания по правому / нижнему краю

    что делаем с листабельным текстом ?



    было бы охуенно если у меня была таблица, просто бегу по количеству ее элементов

    для каждого элемента я знаю как его получить снаружи ??? например

    дальше я могу понять нужно ли его отрисовывать
    дальше могу перерисовать если нужно

*/


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
};

typedef struct {
    xy_t tp;
    struct updated_val uv;
    struct route_time time;
    struct route_since since;
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
    { .color = 0x882222, .l = { .pos_char = { .x = 0,  .y = 0 }, .to_str = route_name    } },

    { .color = 0x555555, .l = { .pos_char = { .x = 30, .y = 0 }, .text = "time:",        } },
    { .color = 0x555555, .l = { .pos_char = { .x = 44, .y = 0 }, .text = ":",            } },
    { .color = 0x555555, .l = { .pos_char = { .x = 47, .y = 0 }, .text = ":",            } },
    { .color = 0x555555, .l = { .pos_char = { .x = 0,  .y = 1 }, .text = "start:",       } },
    { .color = 0x96A41d, .l = { .pos_char = { .x = 21, .y = 1 }, .text = ":",            } },
    { .color = 0x96A41d, .l = { .pos_char = { .x = 24, .y = 1 }, .text = ":",            } },

    { .color = 0x555555, .l = { .pos_char = { .x = 30, .y = 2 }, .text = "dist:",        } },
    { .color = 0x555555, .l = { .pos_char = { .x = 48, .y = 2 }, .text = "km",           } },

    { .color = 0x555555, .l = { .pos_char = { .x = 0,  .y = 2 }, .text = "avg speed:",   } },
    { .color = 0x555555, .l = { .pos_char = { .x = 18, .y = 2 }, .text = "km/h",         } },

    { .color = 0x555555, .l = { .pos_char = { .x = 30, .y = 3 }, .text = "fuel:",        } },
    { .color = 0x555555, .l = { .pos_char = { .x = 49, .y = 3 }, .text = "L",            } },

    { .color = 0x555555, .l = { .pos_char = { .x = 0,  .y = 3 }, .text = "cons:",        } },
    { .color = 0x555555, .l = { .pos_char = { .x = 12, .y = 3 }, .text = "L/h",          } },
    { .color = 0x555555, .l = { .pos_char = { .x = 22, .y = 3 }, .text = "L/100km",      } },

    { .color = 0x555555, .l = { .pos_char = { .x = 31, .y = 1 }, .text = "odo:",         } },
    { .color = 0x555555, .l = { .pos_char = { .x = 48, .y = 1 }, .text = "km",           } },
};

const struct lscolor label_restart[] = {
    { .color = 0xFF0000, .l = { .pos_char = { .x = 14, .y = 0 }, .text = 0,               .len = 13 } },
    { .color = 0xFF0000, .l = { .pos_char = { .x = 14, .y = 0 }, .text = "OK to RESTART", .len = 13 } }
};

const struct lvcolor labels_vals[] = {
    [ROUTE_VALUE_DIST] =        { .color = 0x96A41d, .l = { .pos_char = { .x = 36, .y = 2 }, .len = 11,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_DIST]) } },
    [ROUTE_VALUE_FUEL] =        { .color = 0x96A4Ad, .l = { .pos_char = { .x = 37, .y = 3 }, .len = 11,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_FUEL]) } },
    [ROUTE_VALUE_SINCE_ODO] =   { .color = 0x96A41d, .l = { .pos_char = { .x = 37, .y = 1 }, .len = 10,  .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_SINCE_ODO]) } },
    [ROUTE_VALUE_AVG_SPEED] =   { .color = 0x96A41d, .l = { .pos_char = { .x = 11, .y = 2 }, .len = 6,   .vt = { .f = X1000, .p = 2, .zr = 0}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_AVG_SPEED]) } },
    [ROUTE_VALUE_CONS_DIST] =   { .color = 0x12fa44, .l = { .pos_char = { .x = 16, .y = 3 }, .len = 5,   .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_CONS_DIST]) } },
    [ROUTE_VALUE_CONS_TIME] =   { .color = 0x12fa44, .l = { .pos_char = { .x = 6,  .y = 3 }, .len = 5,   .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_CONS_TIME]) } },
    [ROUTE_VALUE_SINCE_TIME] =  { .l = { .rep = { .vs = VAL_SIZE_32}, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_SINCE_TIME]) } },
    [ROUTE_VALUE_TIME] =        { .l = { .rep = { .vs = VAL_SIZE_32}, .offset = offsetof(struct updated_val, rv[ROUTE_VALUE_TIME]) } },
};

const struct lvcolor labels_route_time[] = {
    { .color = 0xE6A7bd, .l = { .pos_char = { .x = 37, .y = 0 }, .rep = { .vs = VAL_SIZE_32 }, .len = 7, .vt = { .zl = 0 },     .offset = offsetof(struct route_time, h) }, },
    { .color = 0xE6A7bd, .l = { .pos_char = { .x = 45, .y = 0 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .offset = offsetof(struct route_time, m) }, },
    { .color = 0xE6A7bd, .l = { .pos_char = { .x = 48, .y = 0 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .offset = offsetof(struct route_time, s) }, },
};

const struct lvcolor labels_route_since[] = {
    { .color = 0x96A41d, .l = { .pos_char = { .x = 7,  .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 0 },     .offset = offsetof(struct route_since, d.d) },},
    { .color = 0x96A41d, .l = { .pos_char = { .x = 10, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 3, .to_str = month_name,  .offset = offsetof(struct route_since, d.m) }, },
    { .color = 0x96A41d, .l = { .pos_char = { .x = 14, .y = 1 }, .rep = { .vs = VAL_SIZE_16 }, .len = 4, .vt = { .zl = 0 },     .offset = offsetof(struct route_since, d.y) }, },
    { .color = 0x96A41d, .l = { .pos_char = { .x = 19, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .offset = offsetof(struct route_since, t.h) }, },
    { .color = 0x96A41d, .l = { .pos_char = { .x = 22, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .offset = offsetof(struct route_since, t.m) }, },
    { .color = 0x96A41d, .l = { .pos_char = { .x = 25, .y = 1 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .vt = { .zl = 1 },     .offset = offsetof(struct route_since, t.s) }, },
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

static unsigned update_ctx_val(void * ctx, void * new, val_rep_t rep, unsigned offset)
{
    static const uint8_t vl[] = {
        [VAL_SIZE_8] = 1,
        [VAL_SIZE_16] = 2,
        [VAL_SIZE_32] = 4,
    };
    if (!str_cmp(ctx + offset, new + offset, vl[rep.vs])) {
        str_cp(ctx + offset, new + offset, vl[rep.vs]);
        return 1;
    }
    return 0;
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
        .pos = ctx->tp,
    };

    struct updated_val uv;
    ctx_update_vals(&uv, r);
    // printf("update route %d\n", r);

    for (unsigned i = 0; i < ARRAY_SIZE(labels_vals); i++) {
        if (update_ctx_val(&ctx->uv, &uv, labels_vals[i].l.rep, labels_vals[i].l.offset)) {
            if (i == ROUTE_VALUE_TIME) {
                printf("update route time\n");
                struct route_time * rt = &ctx->time;
                struct route_time rt_new;
                ctx_update_time(&rt_new, ctx->uv.rv[ROUTE_VALUE_TIME]);
                for (unsigned j = 0; j < ARRAY_SIZE(labels_route_time); j++) {
                    if (update_ctx_val(rt, &rt_new, labels_route_time[j].l.rep, labels_route_time[j].l.offset)) {
                        cs.fg = labels_route_time[j].color;
                        label_value_print(&tf_ctx, &labels_route_time[j].l, &cs, rt);
                    }
                }
            } else if (i == ROUTE_VALUE_SINCE_TIME) {
                printf("update route since\n");
                struct route_since * rs = &ctx->since;
                struct route_since rs_new;
                ctx_update_since(&rs_new, ctx->uv.rv[ROUTE_VALUE_SINCE_TIME] + time_zone_get());
                for (unsigned j = 0; j < ARRAY_SIZE(labels_route_since); j++) {
                    if (update_ctx_val(rs, &rs_new, labels_route_since[j].l.rep, labels_route_since[j].l.offset)) {
                        cs.fg = labels_route_since[j].color;
                        label_value_print(&tf_ctx, &labels_route_since[j].l, &cs, rs);
                    }
                }
            } else {
                printf("update route value %d\n", i);
                cs.fg = labels_vals[i].color;
                label_value_print(&tf_ctx, &labels_vals[i].l, &cs, &ctx->uv);
            }
        }
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
        .pos = ctx->tp,
    };

    draw_color_form(&el->f, cs.bg);
    for (unsigned i = 0; i < ARRAY_SIZE(label_static); i++) {
        cs.fg = label_static[i].color;
        label_static_print(&tf_ctx, &label_static[i].l, &cs, r);
    }

    ctx_update_vals(&ctx->uv, r);
    for (unsigned i = 0; i < ARRAY_SIZE(labels_vals); i++) {
        if (i == ROUTE_VALUE_TIME) {
            struct route_time * rt = &ctx->time;
            ctx_update_time(rt, ctx->uv.rv[ROUTE_VALUE_TIME]);
            for (unsigned j = 0; j < ARRAY_SIZE(labels_route_time); j++) {
                cs.fg = labels_route_time[j].color;
                label_value_print(&tf_ctx, &labels_route_time[j].l, &cs, rt);
            }
        } else if (i == ROUTE_VALUE_SINCE_TIME) {
            struct route_since * rs = &ctx->since;
            ctx_update_since(rs, ctx->uv.rv[ROUTE_VALUE_SINCE_TIME] + time_zone_get());
            for (unsigned j = 0; j < ARRAY_SIZE(labels_route_since); j++) {
                cs.fg = labels_route_since[j].color;
                label_value_print(&tf_ctx, &labels_route_since[j].l, &cs, rs);
            }
        } else {
            cs.fg = labels_vals[i].color;
            label_value_print(&tf_ctx, &labels_vals[i].l, &cs, &ctx->uv);
        }
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
        .pos = ctx->tp,
    };

    label_static_print(&tf_ctx, &label_restart[ctx->restart_engaged].l, &cs, 0);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    if (event == EVENT_BTN_OK) {
        if (ctx->restart_engaged) {
            printf("reset route %d\n", el->idx);
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
