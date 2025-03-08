#include "widget_time_settings.h"

#include "event_list.h"
#include "draw_color.h"
#include "tc_colors.h"

#include "rtc.h"
#include "time_zone.h"
#include "date_time.h"

#include "val_mod.h"
#include "text_label_color.h"
#include "forms_split.h"
#include "widget_text.h"

/*

что тут я вообще такого ужасного наделал




*/

extern font_t font_5x7;

static const tf_cfg_t title = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .x = 2, .y = 2 },
        .scale = 2
    },
    .limit_char = { .y = 1 },
    .a = ALIGN_LIC,
    .padding = { .x = 6, .y = 4 }
};

static const bg[] = { COLOR(112233), COLOR(0x556677) };

enum {
    TIME_SETTINGS_TIME,
    TIME_SETTINGS_DATE,
    TIME_SETTINGS_TIME_ZONE,
};

typedef struct { unsigned s; time_t t; } uv_time_t;
typedef struct { unsigned days; date_t d; uint8_t dow; } uv_date_t;
typedef struct { int s;  time_t t; } uv_tz_t;

typedef union {
    uv_time_t time;
    uv_date_t date;
    uv_tz_t zone;
} uv_t;

typedef struct {
    tf_ctx_t tf;
    unsigned state;
    uv_t uv;
} ctx_ts_t;


//  часть про изменения значений

typedef struct {
    int min;
    int max;
    int step;
    uint8_t ovf : 1;
} mod_cfg_t;

typedef struct {
    mod_cfg_t cfg;
    const label_t * l;
} uv_mod_t;

typedef struct {
    const uv_mod_t * mod_list;
    void (*prepare)(void * ctx);
    void (*apply)(void * ctx);
    void (*change)(void * ctx);
    uint16_t offset;
    uint8_t count;
} setting_mod_list_t;

static unsigned val_mod(void * ptr, val_rep_t rep, mod_cfg_t * cfg, val_mod_op_t op)
{
    if (rep.s) {
        return val_mod_signed(ptr, rep.vs, op, cfg->ovf, cfg->min, cfg->max, cfg->step);
    } else {
        return val_mod_unsigned(ptr, rep.vs, op, cfg->ovf, cfg->min, cfg->max, cfg->step);
    }
}

void select_val(tf_ctx_t * tf, label_t * l, void * ctx, unsigned select)
{
    const lcd_color_t c[] = { COLOR(0x1144ff), COLOR(0x886612) };
    label_color(tf, &(label_color_t){ .color = c[select], .l = *l }, bg[0], 0, ctx, 0);
}

static void ctx_uv_update_time_s(uv_time_t * ctx, unsigned x)
{
    (void)x;
    ctx->s = rtc_get_time_s() + time_zone_get();
}

static void ctx_uv_update_date(uv_date_t * ctx, unsigned x)
{
    (void)x;
    ctx->days = days_from_s(rtc_get_time_s());
}

static void ctx_uv_update_tz(uv_tz_t * ctx, unsigned x)
{
    (void)x;
    ctx->s = time_zone_get();
}

static void tz_from_s(time_t * t, int tz)
{
    unsigned s = 0;
    if (tz < 0) {
        s = 1;
        tz = -tz;
    }
    time_from_s(t, tz);
    t->s = s;
}

static const label_t labels_time[] = {
    { .t = LP_V, .xy = { .x = -8 }, .rep = { .vs = VAL_SIZE_8 }, .len = 2, .vt = { .zl = 1 }, .ofs = offsetof(time_t, h) },
    { .t = LP_V, .xy = { .x = -5 }, .rep = { .vs = VAL_SIZE_8 }, .len = 2, .vt = { .zl = 1 }, .ofs = offsetof(time_t, m) },
    { .t = LP_V, .xy = { .x = -2 }, .rep = { .vs = VAL_SIZE_8 }, .len = 2, .vt = { .zl = 1 }, .ofs = offsetof(time_t, s) },
};

static const label_t labels_date[] = {
    { .t = LP_V,    .xy = { .x = -11 }, .rep = { .vs = VAL_SIZE_8  }, .len = 2, .ofs = offsetof(date_t, d) },
    { .t = LP_T_FV, .xy = { .x = -7 }, .rep = { .vs = VAL_SIZE_8  }, .len = 3, .ofs = offsetof(date_t, m), .to_str = month_name },
    { .t = LP_V,    .xy = { .x = -2 }, .rep = { .vs = VAL_SIZE_16 }, .len = 4, .ofs = offsetof(date_t, y) },
};

static const label_list_t ll_time_settings_static[] = {
    [TIME_SETTINGS_TIME] = {
        .wrap_list = (label_color_t []) {
            { .color = COLOR(0x1144ff), { .t = LP_T, .text = ":", .xy = { .x = -4 }, .len = 1 } },
            { .color = COLOR(0x1144ff), { .t = LP_T, .text = ":", .xy = { .x = -7 }, .len = 1 } },
        },
        .count = 2,
    },
    [TIME_SETTINGS_DATE] = {},
    [TIME_SETTINGS_TIME_ZONE] = {
        .wrap_list = (label_color_t []) {
            { .color = COLOR(0x1144ff), { .t = LP_T, .text = ":", .xy = { .x = -7 }, .len = 1 } },
        },
        .count = 1,
    },
};

static const char * dow_str(unsigned x)
{
    date_t * d = (date_t *)&x;
    return day_of_week_name(day_of_week(d));
}

static const label_list_t ll_time_settings_vals[] = {
    [TIME_SETTINGS_TIME] = {
        .wrap_list = (label_color_t []) {
            {
                .color = COLOR(0x1144ff),
                .l = {
                    .t = LP_S, .rep = { .vs = VAL_SIZE_32  }, .sofs = offsetof(uv_time_t, t), .ofs = offsetof(uv_time_t, s),
                    .sl = &(const label_list_t){ .ctx_update = (void(*)(void *, unsigned))time_from_s, .list = labels_time, .count = 3 }
                }
            },
        },
        .count = 1,
        .ctx_update = (void(*)(void *, unsigned))ctx_uv_update_time_s
    },
    [TIME_SETTINGS_DATE] = {
        .wrap_list = (label_color_t []) {
            {
                .color = COLOR(0x1144ff),
                .l = {
                    .t = LP_S, .rep = { .vs = VAL_SIZE_32 }, .sofs = offsetof(uv_date_t, d), .ofs = offsetof(uv_date_t, days),
                    .sl = &(const label_list_t){ .ctx_update = (void(*)(void *, unsigned))date_from_days, .list = labels_date, .count = 3 }
                }
            },
                // { .t = , .xy = { .x = 0  - 15 }, .ofs = offsetof(ctx_t, dow), .vt = { .l = 3, }, .to_str = day_of_week_name },
        },
        .count = 1,
        .ctx_update = (void(*)(void *, unsigned))ctx_uv_update_date
    },
    [TIME_SETTINGS_TIME_ZONE] = {
        .wrap_list = (label_color_t []) {
            {
                .color = COLOR(0x1144ff),
                .l = {
                    .t = LP_S, .rep = { .s = 1, .vs = VAL_SIZE_32 }, .sofs = offsetof(uv_tz_t, t), .ofs = offsetof(uv_tz_t, s),
                    .sl = &(const label_list_t){
                        .ctx_update_signed = (void(*)(void *, int))tz_from_s, .count = 3,
                        .list = (const label_t []) {
                            { .xy = { .x = -10 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .t = LP_T_LV, .text_list = (const char * []){ "+", "-" }, .ofs = offsetof(time_t, s) },
                            { .xy = { .x = -8  }, .rep = { .vs = VAL_SIZE_8 }, .len = 2, .t = LP_V, .vt = { .zl = 1 }, .ofs = offsetof(time_t, h) },
                            { .xy = { .x = -5  }, .rep = { .vs = VAL_SIZE_8 }, .len = 2, .t = LP_V, .vt = { .zl = 1 }, .ofs = offsetof(time_t, m) },
                        }
                    }
                }
            }
        },
        .count = 1,
        .ctx_update = (void(*)(void *, unsigned))ctx_uv_update_tz
    }
};

static void mod_prepare_time(ctx_ts_t * ctx)
{
    ctx->uv.time.t.s = 0;
    select_val(&ctx->tf, &labels_time[2], &ctx->uv.time.t, 0);
}

static void mod_apply_time(ctx_ts_t * ctx)
{
    int tz = time_zone_get();
    unsigned time_s = rtc_get_time_s() + tz;
    rtc_set_time_s(time_change_in_s(&ctx->uv.time.t, time_s) - tz);
}

static mod_apply_date(ctx_ts_t * ctx)
{
    int tz = time_zone_get();
    unsigned time_s = rtc_get_time_s() + tz;
    rtc_set_time_s(date_change_in_s(&ctx->uv.date.d, time_s) - tz);
}

static void mod_apply_tz(ctx_ts_t * ctx)
{
    time_zone_set(ctx->uv.zone.s);
}

static const setting_mod_list_t mod[] = {
    {
        .apply = mod_apply_time,
        .prepare = mod_prepare_time,
        .count = 2,
        .offset = offsetof(ctx_ts_t, uv.time.t),
        .mod_list = (uv_mod_t []) {
            { .cfg = { .min = 0,    .max = 23,   .step = 1, .ovf = 1 }, .l = &labels_time[0] },
            { .cfg = { .min = 0,    .max = 59,   .step = 1, .ovf = 1 }, .l = &labels_time[1] }
        }
    },
    {
        .count = 3,
        .offset = offsetof(ctx_ts_t, uv.date.d),
        .mod_list = (uv_mod_t []) {
            { .cfg = { .min = 1,    .max = 31,   .step = 1, .ovf = 1 }, .l = &labels_date[0] },
            { .cfg = { .min = 0,    .max = 11,   .step = 1, .ovf = 1 }, .l = &labels_date[1] },
            { .cfg = { .min = 2000, .max = 2100, .step = 1, .ovf = 0 }, .l = &labels_date[2] },
        }
    },
    {
        .count = 1,
        .offset = offsetof(ctx_ts_t, uv.zone.s),
        .apply = mod_apply_tz,
        .mod_list = (uv_mod_t []) {
            {
                .cfg = { .min = -12 * 60 * 60, .max = 12 * 60 * 60, .step = 15 * 60, .ovf = 0 },
                .l = &(((label_color_t *)(ll_time_settings_vals[TIME_SETTINGS_TIME_ZONE].wrap_list))[0].l)
            },
        }
    }
};

static void update_time_setting(ui_element_t * el)
{
    ctx_ts_t * ctx = (ctx_ts_t *)el->ctx;
    if (ctx->state == 0) {
        uv_t uv;
        label_color_list(&ctx->tf, &ll_time_settings_vals[el->idx], bg[0], 0, &uv, &ctx->uv);
    }
}

static void draw_time_setting(ui_element_t * el)
{
    ctx_ts_t * ctx = (ctx_ts_t *)el->ctx;
    if (tf_ctx_calc(&ctx->tf, &el->f, &title)) {
        el->drawed = 1;
        ctx->state = 0;
        draw_color_form(&el->f, bg[0]);
        label_color_list(&ctx->tf, &ll_time_settings_static[el->idx], bg[0], 0, 0, 0);
        label_color_list(&ctx->tf, &ll_time_settings_vals[el->idx], bg[0], 0, &ctx->uv, 0);
    }
}

static unsigned process_time_setting(ui_element_t * el, unsigned event)
{
    ctx_ts_t * ctx = (ctx_ts_t *)el->ctx;
    unsigned new_state = ctx->state;
    uv_mod_t * m = &mod[el->idx].mod_list[ctx->state - 1];
    void * uv = ctx;
    uv += mod[el->idx].offset;

    if (ctx->state) {
        void * vp = uv + m->l->ofs;

        if (event == EVENT_BTN_UP) {
            val_mod(vp, m->l->rep, &m->cfg, MOD_OP_ADD);
            select_val(&ctx->tf, m->l, uv, 1);
            if (mod[el->idx].change) {
                mod[el->idx].change(ctx);
            }
            return 1;
        }

        if (event == EVENT_BTN_DOWN) {
            val_mod(vp, m->l->rep, &m->cfg, MOD_OP_SUB);
            select_val(&ctx->tf, m->l, uv, 1);
            if (mod[el->idx].change) {
                mod[el->idx].change(ctx);
            }
            return 1;
        }

        if (event == EVENT_BTN_BACK) {
            new_state--;
        }
    }
    if (event == EVENT_BTN_OK) {
        new_state++;
    }

    if (ctx->state != new_state) {
        if (new_state > mod[el->idx].count) {
            new_state = 0;
            if (mod[el->idx].apply) {
                mod[el->idx].apply(ctx);
            }
        }

        if (new_state) {
            select_val(&ctx->tf, mod[el->idx].mod_list[new_state - 1].l, uv, 1);
        }
        if (ctx->state) {
            select_val(&ctx->tf, m->l, uv, 0);
        } else {
            if (mod[el->idx].prepare) {
                mod[el->idx].prepare(ctx);
            }
        }
        ctx->state = new_state;
        return 1;
    }

    return 0;
}

const widget_desc_t widget_time_setting = {
    .draw = draw_time_setting,
    .update = update_time_setting,
    .process_event = process_time_setting,
    .ctx_size = sizeof(ctx_ts_t)
};

static const ui_node_desc_t time_setting_node = {
    .widget = &widget_time_setting,
};

typedef struct {
    tf_ctx_t title;
} ctx_settings_title_t;

static const label_color_t tz_title = { .color = COLOR(0x11ff44), .l = { .text_list = (const char * []){ "TIME", "DATE", "TIME ZONE" }, .xy = { .x = 2 }, .t = LP_T_LIDX } };

static const label_list_t title_selector = {
    .wrap_list = (label_color_t []) {
        { .color = COLOR(0x11ff44), .l = { .xy = { .x =  0 }, .len = 1, .text_list = (const char *[]){ 0, "[" }, .t = LP_T_LIDX } },
        { .color = COLOR(0x11ff44), .l = { .xy = { .x = 12 }, .len = 1, .text_list = (const char *[]){ 0, "]" }, .t = LP_T_LIDX } },
    },
    .count = 2
};

static void select(ui_element_t * el)
{
    ctx_settings_title_t * ctx = (ctx_settings_title_t *)el->ctx;
    label_color_list(&ctx->title, &title_selector, bg[0], el->active, 0, 0);
    ui_tree_element_select(ui_tree_child(el), el->active);
}

static void draw(ui_element_t * el)
{
    ctx_settings_title_t * ctx = (ctx_settings_title_t *)el->ctx;
    form_t f_title = el->f;
    // тут надо просто сначала одну форму посчитать, потом дочернего элмемента и склеить их размеры
    if (tf_ctx_calc(&ctx->title, &f_title, &title)) {

        ui_element_t * item = ui_tree_add(el, &time_setting_node, el->idx);
        form_cut(&item->f, DIMENSION_Y, EDGE_U, f_title.s.h);
        ui_tree_element_draw(item);

        if (item->drawed) {
            form_reduce(&el->f, DIMENSION_Y, EDGE_U, f_title.s.h + item->f.s.h);

            draw_color_form(&f_title, bg[0]);
            // draw_color_form(&el->f, bg[0]);

            label_color(&ctx->title, &tz_title, bg[0], el->idx, 0, 0);
            select(el);

            el->drawed = 1;
        }
    }
}

const widget_desc_t widget_settings_title = {
    .select = select,
    .draw = draw,
    .ctx_size = sizeof(ctx_settings_title_t)
};
