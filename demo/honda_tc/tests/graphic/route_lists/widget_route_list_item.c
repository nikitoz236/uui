#include "widget_route_list_item.h"
#include "lcd_text_color.h"
#include "text_field.h"
#include "draw_color.h"
#include "array_size.h"
#include "val_text.h"
#include "str_val_buf.h"
#include <stddef.h>

#include "routes.h"
#include "date_time.h"
#include "time_zone.h"

#include "event_list.h"

/*
    достаточно типичная ситуация - горизонтальный элемент
    как выглядит работа с ним

    на стадии сфдс вычисляется размер формы - дальше форму такто надо сохранить если это например заголовок чтобы можно было ее закрасить на стадии draw

*/


const lcd_color_t bg[] = { 0x111111, 0x113222 };

typedef struct {
    xy_t tp;
    unsigned rv[ROUTE_VALUE_NUM];
    unsigned time_h;
    uint8_t time_s;
    uint8_t time_m;
    time_t since_t;
    date_t since_d;
    uint8_t restart_engaged;
} ctx_t;

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

enum {
    ITEM_DIST
};

struct label {
    text_field_label_t tl;
    lcd_color_t color;
    enum { LABEL_TEXT = 0, LABEL_STR_FORM_FUNC_BY_IDX, LABEL_VAL_FORM_OFFSET } type;
};

const struct label labels[] = {
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 30, .y = 0 }, .text = "time:",         } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 44, .y = 0 }, .text = ":",             } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 47, .y = 0 }, .text = ":",             } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 1 }, .text = "start:",        } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 21, .y = 1 }, .text = ":",             } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 24, .y = 1 }, .text = ":",             } },

    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 30, .y = 2 }, .text = "dist:",         } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 48, .y = 2 }, .text = "km",            } },

    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 2 }, .text = "avg speed:",    } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 18, .y = 2 }, .text = "km/h",          } },
    // { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 11, .y = 2 }, .text = "253.45",        } },

    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 30, .y = 3 }, .text = "fuel:",         } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 49, .y = 3 }, .text = "L",             } },

    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 3 }, .text = "cons:",         } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 12, .y = 3 }, .text = "L/h",           } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 22, .y = 3 }, .text = "L/100km",       } },

    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 31, .y = 1 }, .text = "odo:",          } },
    // { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 1 }, .text = "345674.323",    } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 48, .y = 1 }, .text = "km",            } },

    { .color = 0x882222, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 0 }, .to_str = route_name }, .type = LABEL_STR_FORM_FUNC_BY_IDX },
};

const struct label label_restart = {
      .color = 0xFF0000, .tl = { .tfcfg = &tf, .pos = { .x = 14, .y = 0 }, .text = "OK to RESTART", .len = 13 }
};

const struct label labels_vals[] = {
    [ROUTE_VALUE_DIST] =        { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 36, .y = 2 }, .len = 11, .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(ctx_t, rv[ROUTE_VALUE_DIST]) }, .type = LABEL_VAL_FORM_OFFSET },
    [ROUTE_VALUE_FUEL] =        { .color = 0x96A4Ad, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 3 }, .len = 11, .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(ctx_t, rv[ROUTE_VALUE_FUEL]) }, .type = LABEL_VAL_FORM_OFFSET },
    [ROUTE_VALUE_SINCE_ODO] =   { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 1 }, .len = 10, .vt = { .f = X1000, .p = 3, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(ctx_t, rv[ROUTE_VALUE_SINCE_ODO]) }, .type = LABEL_VAL_FORM_OFFSET },
    [ROUTE_VALUE_AVG_SPEED] =   { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 11, .y = 2 }, .len = 6,  .vt = { .f = X1000, .p = 2, .zr = 0}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(ctx_t, rv[ROUTE_VALUE_AVG_SPEED]) }, .type = LABEL_VAL_FORM_OFFSET },
    [ROUTE_VALUE_CONS_DIST] =   { .color = 0x12fa44, .tl = { .tfcfg = &tf, .pos = { .x = 16, .y = 3 }, .len = 5,  .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(ctx_t, rv[ROUTE_VALUE_CONS_DIST]) }, .type = LABEL_VAL_FORM_OFFSET },
    [ROUTE_VALUE_CONS_TIME] =   { .color = 0x12fa44, .tl = { .tfcfg = &tf, .pos = { .x = 6,  .y = 3 }, .len = 5, .vt = { .f = X1000, .p = 2, .zr = 1}, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(ctx_t, rv[ROUTE_VALUE_CONS_TIME]) }, .type = LABEL_VAL_FORM_OFFSET },
};

enum {
    LABEL_ROUTE_TIME_H,
    LABEL_ROUTE_TIME_M,
    LABEL_ROUTE_TIME_S,
    LABEL_ROUTE_SINCE_DAY,
    LABEL_ROUTE_SINCE_MONTH,
    LABEL_ROUTE_SINCE_YEAR,
    LABEL_ROUTE_SINCE_H,
    LABEL_ROUTE_SINCE_M,
    LABEL_ROUTE_SINCE_S
};

const struct label labels_route_time[] = {
    [LABEL_ROUTE_TIME_H] =      { .color = 0xE6A7bd, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 0 }, .len = 7, .vt = { .zl = 0 }, .rep = { .vs = VAL_SIZE_32 }, .offset = offsetof(ctx_t, time_h) },    .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_TIME_M] =      { .color = 0xE6A7bd, .tl = { .tfcfg = &tf, .pos = { .x = 45, .y = 0 }, .len = 2, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .offset = offsetof(ctx_t, time_m) },    .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_TIME_S] =      { .color = 0xE6A7bd, .tl = { .tfcfg = &tf, .pos = { .x = 48, .y = 0 }, .len = 2, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .offset = offsetof(ctx_t, time_s) },    .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_SINCE_DAY] =   { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 7,  .y = 1 }, .len = 2, .vt = { .zl = 0 }, .rep = { .vs = VAL_SIZE_8 }, .offset = offsetof(ctx_t, since_d.d) }, .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_SINCE_MONTH] = { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 10, .y = 1 }, .len = 3, .to_str = month_name, .rep = { .vs = VAL_SIZE_8 }, .offset = offsetof(ctx_t, since_d.m) }, .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_SINCE_YEAR] =  { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 14, .y = 1 }, .len = 4, .vt = { .zl = 0 }, .rep = { .vs = VAL_SIZE_16 }, .offset = offsetof(ctx_t, since_d.y) }, .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_SINCE_H] =     { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 19, .y = 1 }, .len = 2, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .offset = offsetof(ctx_t, since_t.h) }, .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_SINCE_M] =     { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 22, .y = 1 }, .len = 2, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .offset = offsetof(ctx_t, since_t.m) }, .type = LABEL_VAL_FORM_OFFSET },
    [LABEL_ROUTE_SINCE_S] =     { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 25, .y = 1 }, .len = 2, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .offset = offsetof(ctx_t, since_t.s) }, .type = LABEL_VAL_FORM_OFFSET },

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

static void print_label(const struct label * l, xy_t pos, unsigned idx, unsigned active, void * ctx)
{
    char * str = l->tl.text;
    if (l->type == LABEL_STR_FORM_FUNC_BY_IDX) {
        str = l->tl.to_str(idx);
    } else if (l->type == LABEL_VAL_FORM_OFFSET) {
        if (l->tl.to_str) {
            str = l->tl.to_str(val_unpack(ctx + l->tl.offset, l->tl.rep, 0));
        } else {
            str = str_val_buf_get();
            val_text_ptr_to_str(str, ctx + l->tl.offset, l->tl.rep, l->tl.vt, l->tl.len);
        }
    }
    color_scheme_t cs = { .fg = l->color, .bg = bg[active] };
    lcd_color_text_raw_print(str, l->tl.tfcfg->fcfg, &cs, &pos, &l->tl.tfcfg->limit_char, &l->tl.pos, l->tl.len);
}

static void print_labels(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->rv[ROUTE_VALUE_DIST] = 7654321;
    for (unsigned i = 0; i < ARRAY_SIZE(labels); i++) {
        print_label(&labels[i], ctx->tp, el->idx, el->active, ctx);
    }
}

static void calc(ui_element_t * el)
{
    el->f.s = text_field_size_px(&tf);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->tp = text_field_text_pos(&el->f, &tf);
}

static void draw_route_val(ctx_t * ctx, route_value_t t, unsigned active)
{
    if (t == ROUTE_VALUE_TIME) {
        unsigned time_s = ctx->rv[ROUTE_VALUE_TIME];
        uint8_t s = time_s % 60;
        if (ctx->time_s != s) {
            ctx->time_s = s;
            print_label(&labels_route_time[LABEL_ROUTE_TIME_S], ctx->tp, 0, active, ctx);
        }
        time_s /= 60;
        uint8_t m = time_s % 60;
        if (ctx->time_m != m) {
            ctx->time_m = m;
            print_label(&labels_route_time[LABEL_ROUTE_TIME_M], ctx->tp, 0, active, ctx);
        }
        unsigned h = time_s / 60;
        if (ctx->time_h != h) {
            ctx->time_h = h;
            print_label(&labels_route_time[LABEL_ROUTE_TIME_H], ctx->tp, 0, active, ctx);
        }
    } else if (t == ROUTE_VALUE_SINCE_TIME) {
        unsigned since_s = ctx->rv[ROUTE_VALUE_SINCE_TIME] + time_zone_get();
        date_t d;
        date_from_s(&d, since_s);
        time_t t;
        time_from_s(&t, since_s);
        if (ctx->since_d.d != d.d) {
            ctx->since_d.d = d.d;
            print_label(&labels_route_time[LABEL_ROUTE_SINCE_DAY], ctx->tp, 0, active, ctx);
        }
        if (ctx->since_d.m != d.m) {
            ctx->since_d.m = d.m;
            print_label(&labels_route_time[LABEL_ROUTE_SINCE_MONTH], ctx->tp, 0, active, ctx);
        }
        if (ctx->since_d.y != d.y) {
            ctx->since_d.y = d.y;
            print_label(&labels_route_time[LABEL_ROUTE_SINCE_YEAR], ctx->tp, 0, active, ctx);
        }
        if (ctx->since_t.h != t.h) {
            ctx->since_t.h = t.h;
            print_label(&labels_route_time[LABEL_ROUTE_SINCE_H], ctx->tp, 0, active, ctx);
        }
        if (ctx->since_t.m != t.m) {
            ctx->since_t.m = t.m;
            print_label(&labels_route_time[LABEL_ROUTE_SINCE_M], ctx->tp, 0, active, ctx);
        }
        if (ctx->since_t.s != t.s) {
            ctx->since_t.s = t.s;
            print_label(&labels_route_time[LABEL_ROUTE_SINCE_S], ctx->tp, 0, active, ctx);
        }
    } else {
        print_label(&labels_vals[t], ctx->tp, 0, active, ctx);
    }
}

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    route_t r = el->idx;
    for (route_value_t t = 0; t < ROUTE_VALUE_NUM; t++) {
        unsigned v = route_get_value(r, t);
        if (ctx->rv[t] != v) {
            ctx->rv[t] = v;
            draw_route_val(ctx, t, el->active);
        }
    }
}

/*
    нужны функции draw_val и draw_val_if_changed (update_val)
    -1 это какойто колхоз
*/

static void draw(ui_element_t * el)
{
    draw_color_form(&el->f, bg[el->active]);
    print_labels(el);
    ctx_t * ctx = (ctx_t *)el->ctx;
    route_t r = el->idx;
    ctx->time_h = -1;
    ctx->time_m = -1;
    ctx->time_s = -1;
    ctx->since_d = (date_t){ .y = -1, .m = -1, .d = -1 };
    ctx->since_t = (time_t){ .h = -1, .m = -1, .s = -1 };
    ctx->restart_engaged = 0;
    for (route_value_t t = 0; t < ROUTE_VALUE_NUM; t++) {
        unsigned v = route_get_value(r, t);
        ctx->rv[t] = v;
        draw_route_val(ctx, t, el->active);
    }
    printf("route item %s time %d dist %d avg speed %d\n", route_name(r), ctx->rv[ROUTE_VALUE_TIME], ctx->rv[ROUTE_VALUE_DIST], ctx->rv[ROUTE_VALUE_AVG_SPEED]);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    if (event == EVENT_BTN_OK) {
        if (ctx->restart_engaged) {
            printf("reset route %d\n", el->idx);
            route_reset(el->idx);
            ctx->restart_engaged = 0;
            lcd_color_text_raw_print(0, label_restart.tl.tfcfg->fcfg, &(color_scheme_t){ .bg = bg[el->active] }, &ctx->tp, 0, &label_restart.tl.pos, label_restart.tl.len);
            update(el);
        } else {
            if ((el->idx == ROUTE_TYPE_TRIP) || (el->idx == ROUTE_TYPE_TOTAL) || (el->idx == ROUTE_TYPE_DAY)) {
                return 1;
            }

            ctx->restart_engaged = 1;
            print_label(&label_restart, ctx->tp, 0, el->active, ctx);
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
