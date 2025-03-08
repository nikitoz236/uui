#include "widget_time_settings.h"
#include "lcd_text.h"
#include "lcd_text_color.h"
#include "event_list.h"
#include "tc_colors.h"
#include "date_time.h"
#include "rtc.h"
#include "align_forms.h"
#include "draw_color.h"
#include "str_val.h"
#include "val_pack.h"

#include "val_mod.h"

#include "time_zone.h"
#include "text_field.h"


typedef struct {
    union {
        uint16_t u_min;
        int16_t s_min;
    };
    union {
        uint16_t u_max;
        int16_t s_max;
    };
    union {
        uint16_t u_step;
        int16_t s_step;
    };
    uint8_t ovf : 1;
} mod_cfg_t;

static unsigned val_mod(void * ptr, val_rep_t rep, mod_cfg_t * cfg, val_mod_op_t op)
{
    if (rep.s) {
        return val_mod_signed(ptr, rep.vs, op, cfg->ovf, cfg->s_min, cfg->s_max, cfg->s_step);
    } else {
        return val_mod_unsigned(ptr, rep.vs, op, cfg->ovf, cfg->u_min, cfg->u_max, cfg->u_step);
    }
}

typedef struct {
    mod_cfg_t cfg;
    label_t * l;
} uv_mod_t;

extern font_t font_5x7;

/*
что общего

оффсет в структуре
также нужно представление числа в памяти. после этого по офсету и представлению можно получить значение
дальше нужно его напечатать

text_field_label_t

варианты
    - тупо статический текст по указателю
    - с использованием val-text преобразовать в строку
    - получить строку по индексу

    может быть нексколько индексов как в случае с маршрутами


а еще оно должно както там оказаться. должна быть функция которая там его поместит, хмм

*/


/*


const val_text_updatable_t vtu_time[] = {
    [VTU_H]     = { .tfcfg =  &tf, .pos_char = { .x = 0  }, .offset_in_ctx = offsetof(ctx_t, time.h), .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 0}, .min = 0,    .max = 23,   .step = 1, .ovf = 1 },
    [VTU_M]     = { .tfcfg =  &tf, .pos_char = { .x = 3  }, .offset_in_ctx = offsetof(ctx_t, time.m), .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 1}, .min = 0,    .max = 59,   .step = 1, .ovf = 1 },
    [VTU_S]     = { .tfcfg =  &tf, .pos_char = { .x = 6  }, .offset_in_ctx = offsetof(ctx_t, time.s), .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 1}, },
};

const val_text_updatable_t vtu_date[] = {
    [VTU_YEAR]  = { .tfcfg =  &tf, .pos_char = { .x = 11 }, .offset_in_ctx = offsetof(ctx_t, date.y), .vt = { .l = 4, .vs = VAL_SIZE_16, .zl = 0}, .min = 2000, .max = 2100, .step = 1, .ovf = 0 },
    [VTU_DAY]   = { .tfcfg =  &tf, .pos_char = { .x = 4  }, .offset_in_ctx = offsetof(ctx_t, date.d), .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 0}, .min = 1,    .max = 31,   .step = 1, .ovf = 1 },
    [VTU_MONTH] = { .tfcfg =  &tf, .pos_char = { .x = 7  }, .offset_in_ctx = offsetof(ctx_t, date.m), .vt = { .l = 3, .vs = VAL_SIZE_8,  .zl = 0}, .min = 0,    .max = 11,   .step = 1, .ovf = 1, .to_str = month_name },
    [VTU_DOW]   = { .tfcfg =  &tf, .pos_char = { .x = 0  }, .offset_in_ctx = offsetof(ctx_t, dow), .vt = { .l = 3, }, .to_str = day_of_week_name },
};

const val_text_updatable_t vtu_tz[] = {
    [VTU_H]     = { .tfcfg =  &tf, .pos_char = { .x = 2  }, .offset_in_ctx = offsetof(ctx_t, tz_time.h), .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 0} },
    [VTU_M]     = { .tfcfg =  &tf, .pos_char = { .x = 5  }, .offset_in_ctx = offsetof(ctx_t, tz_time.m), .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 1} },
    [VTU_TZ_SIGN] = { .tfcfg =  &tf, .pos_char = { .x = 0  }, .offset_in_ctx = offsetof(ctx_t, tz_sign), .vt = { .l = 1, .vs = VAL_SIZE_8 }, .to_str = tz_sign_to_str },
};

static void update_vtu(ctx_t * ctx, const val_text_updatable_t * vtu, color_scheme_t * cs)
{
    void * ptr = ctx;
    ptr += vtu->offset_in_ctx;

    const char * str;

    if (vtu->to_str) {
        unsigned x = val_ptr_to_usnigned(ptr, vtu->vt.vs);
        str = vtu->to_str(x);
    } else {
        str = str_val_buf_get();
        val_text_ptr_to_str(str, ptr, )
        // val_text_ptr_to_str(str, ctx + l->tl.offset, l->tl.rep, l->tl.vt, l->tl.len);
        // val_text_to_str((char*)str, ptr, &vtu->vt);
    }

    lcd_color_text_raw_print(str, vtu->tfcfg->fcfg, cs, &ctx->text_pos, &vtu->tfcfg->limit_char, &vtu->pos_char, vtu->vt.l);
}

static void mod_vtu(ctx_t * ctx, const val_text_updatable_t * vtu, val_mod_op_t op)
{
    void * ptr = ctx;
    ptr += vtu->offset_in_ctx;
    if( val_mod_unsigned(ptr, vtu->vt.vs, op, vtu->ovf, vtu->min, vtu->max, vtu->step)) {
        update_vtu(ctx, vtu, cs(1, 1));
    }
}

static void update_time(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    if (ctx->vtu == VTU_NONE) {
        unsigned current_time_s = rtc_get_time_s() + time_zone_get();
        if (ctx->current_time_s != current_time_s) {
            ctx->current_time_s = current_time_s;
            time_from_s(&ctx->time, current_time_s);

            update_vtu(ctx, &vtu_time[VTU_H], cs(el->active, 0));
            update_vtu(ctx, &vtu_time[VTU_M], cs(el->active, 0));
            update_vtu(ctx, &vtu_time[VTU_S], cs(el->active, 0));
        }
    }
}

static void redraw_time_widget(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    draw_color_form(&el->f, cs(el->active, 0)->bg);
    lcd_color_text_raw_print("Time set:", &fcfg, cs(el->active, 0), &ctx->title_pos, 0, 0, 0);
    lcd_color_text_raw_print(":", &fcfg, cs(el->active, 0), &ctx->text_pos, 0, &(xy_t){ .x = 2 }, 1);
    lcd_color_text_raw_print(":", &fcfg, cs(el->active, 0), &ctx->text_pos, 0, &(xy_t){ .x = 5 }, 1);

    ctx->current_time_s = -1;
    update_time(el);
}

static unsigned process_time(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("process_time %d\n", event);
    unsigned change_vtu = ctx->vtu;

    if (ctx->vtu == VTU_NONE) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_H;
            ctx->time.s = 0;
            update_vtu(ctx, &vtu_time[VTU_H], cs(1, 1));
            update_vtu(ctx, &vtu_time[VTU_S], cs(1, 0));
            return 1;
        }
    } else {
        if (event == EVENT_BTN_OK) {
            static unsigned next_vtu_date[] = {
                [VTU_H] = VTU_M,
                [VTU_M] = VTU_NONE,
            };
            change_vtu = next_vtu_date[ctx->vtu];
            if (change_vtu == VTU_NONE) {
                // set time
                int tz = time_zone_get();
                unsigned time_s = rtc_get_time_s() + tz;
                rtc_set_time_s(time_change_in_s(&ctx->time, time_s) - tz);
            }
        }

        if (event == EVENT_BTN_LEFT) {
            static unsigned next_vtu_date[] = {
                [VTU_H] = VTU_NONE,
                [VTU_M] = VTU_H,
            };
            change_vtu = next_vtu_date[ctx->vtu];
            if (ctx->vtu == VTU_H) {
                change_vtu = VTU_NONE;
            } else if (ctx->vtu == VTU_M) {
                change_vtu = VTU_NONE;
            }
            if (change_vtu == VTU_NONE) {
                ctx->current_time_s = -1;
            }
        }

        if (change_vtu != ctx->vtu) {
            if (change_vtu == VTU_NONE) {
                ctx->vtu = VTU_NONE;
                update_time(el);
            } else {
                update_vtu(ctx, &vtu_time[ctx->vtu], cs(1, 0));
                ctx->vtu = change_vtu;
                update_vtu(ctx, &vtu_time[ctx->vtu], cs(1, 1));
            }
            return 1;
        }

        if (event == EVENT_BTN_UP) {
            mod_vtu(ctx, &vtu_time[ctx->vtu], MOD_OP_ADD);
            return 1;
        }

        if (event == EVENT_BTN_DOWN) {
            mod_vtu(ctx, &vtu_time[ctx->vtu], MOD_OP_SUB);
            return 1;
        }
    }
    return 0;
}

void calc_date(ui_element_t * el)
{
    //  Date setup
    //  TUE 21 APR 2000
    //  0123456789012345
    el->f.s = calc_setting_form_size(TEXT_LEN("Date set:"));
}

static void extend_date(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->vtu = VTU_NONE;
    calc_pos(ctx, &el->f, TEXT_LEN("Date set:"), TEXT_LEN("TUE 21 APR 2000"));
}

static void update_dow(ctx_t * ctx, color_scheme_t * cs)
{
    unsigned dow = day_of_week(&ctx->date);
    if (ctx->dow != dow) {
        ctx->dow = dow;
        update_vtu(ctx, &vtu_date[VTU_DOW], cs);
    }
}

static void update_date(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("update date, current day %d vtu %d\n", ctx->current_day, ctx->vtu);
    if (ctx->vtu == VTU_NONE) {
        unsigned current_day = days_from_s(rtc_get_time_s() + time_zone_get());
        if (ctx->current_day != current_day) {
            ctx->current_day = current_day;
            date_from_days(&ctx->date, current_day);

            update_vtu(ctx, &vtu_date[VTU_DAY], cs(el->active, 0));
            update_vtu(ctx, &vtu_date[VTU_MONTH], cs(el->active, 0));
            update_vtu(ctx, &vtu_date[VTU_YEAR], cs(el->active, 0));
            update_dow(ctx, cs(el->active, 0));
        }
    }
}

static void redraw_date_widget(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    draw_color_form(&el->f, cs(el->active, 0)->bg);
    lcd_color_text_raw_print("Date set:", &fcfg, cs(el->active, 0), &ctx->title_pos, 0, 0, 0);
    // printf("redraw date widget, active %d\n", el->active);
    ctx->current_day = -1;
    ctx->dow = -1;
    update_date(el);
}

static unsigned process_date(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("process date %d\n", event);
    unsigned change_vtu = ctx->vtu;

    if (ctx->vtu == VTU_NONE) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_YEAR;
            update_vtu(ctx, &vtu_date[ctx->vtu], cs(1, 1));
            return 1;
        }
    } else {
        if (event == EVENT_BTN_OK) {
            unsigned next_vtu_date[] = {
                [VTU_YEAR] = VTU_MONTH,
                [VTU_MONTH] = VTU_DAY,
                [VTU_DAY] = VTU_NONE
            };
            change_vtu = next_vtu_date[ctx->vtu];
            if (change_vtu == VTU_NONE) {
                // set date
                int tz = time_zone_get();
                unsigned time_s = rtc_get_time_s() + tz;
                rtc_set_time_s(date_change_in_s(&ctx->date, time_s) - tz);
            }
        }

        if (event == EVENT_BTN_LEFT) {
            static unsigned next_vtu_date[] = {
                [VTU_YEAR] = VTU_NONE,
                [VTU_MONTH] = VTU_YEAR,
                [VTU_DAY] = VTU_MONTH
            };
            change_vtu = next_vtu_date[ctx->vtu];
            if (change_vtu == VTU_NONE) {
                ctx->current_day = -1;
            }
        }

        if (change_vtu != ctx->vtu) {
            if (change_vtu == VTU_NONE) {
                ctx->vtu = VTU_NONE;
                update_date(el);
            } else {
                update_vtu(ctx, &vtu_date[ctx->vtu], cs(1, 0));
                ctx->vtu = change_vtu;
                update_vtu(ctx, &vtu_date[ctx->vtu], cs(1, 1));
            }
            return 1;
        }

        if (event == EVENT_BTN_UP) {
            mod_vtu(ctx, &vtu_date[ctx->vtu], MOD_OP_ADD);
            update_dow(ctx, cs(el->active, 0));
            return 1;
        }

        if (event == EVENT_BTN_DOWN) {
            mod_vtu(ctx, &vtu_date[ctx->vtu], MOD_OP_SUB);
            update_dow(ctx, cs(el->active, 0));
            return 1;
        }
    }
    return 0;
}

static void update_tz(ctx_t * ctx, color_scheme_t * cs)
{
    if (ctx->tz_s < 0) {
        ctx->tz_sign = 1;
        time_from_s(&ctx->tz_time, -ctx->tz_s);
    } else {
        ctx->tz_sign = 0;
        time_from_s(&ctx->tz_time, ctx->tz_s);
    }

    update_vtu(ctx, &vtu_tz[VTU_TZ_SIGN], cs);
    update_vtu(ctx, &vtu_tz[VTU_H], cs);
    update_vtu(ctx, &vtu_tz[VTU_M], cs);

    lcd_color_text_raw_print(":", &fcfg, cs, &ctx->text_pos, 0, &(xy_t){ .x = 4 }, 1);
}

static void redraw_tz_widget(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    draw_color_form(&el->f, cs(el->active, 0)->bg);
    lcd_color_text_raw_print("Time zone:", &fcfg, cs(el->active, 0), &ctx->title_pos, 0, 0, 0);

    ctx->tz_s = time_zone_get();
    update_tz(ctx, cs(el->active, 0));
}

static void mod_tz(ctx_t * ctx, val_mod_op_t op)
{
    if (val_mod_signed(&ctx->tz_s, VAL_SIZE_32, op, 0, -12 * 60 * 60, 12 * 60 * 60, 15 * 60)) {
        update_tz(ctx, cs(1, 1));
    }
}

static unsigned process_tz(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("process date %d\n", event);

    if (ctx->vtu == VTU_NONE) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = 1;
            update_tz(ctx, cs(1, 1));
            return 1;
        }
    } else {
        if (event == EVENT_BTN_OK) {
            time_zone_set(ctx->tz_s);
            update_tz(ctx, cs(1, 0));
            ctx->vtu = VTU_NONE;
            return 1;
        }

        if (event == EVENT_BTN_LEFT) {
            ctx->tz_s = time_zone_get();
            update_tz(ctx, cs(1, 0));
            ctx->vtu = VTU_NONE;
            return 1;
        }

        if (event == EVENT_BTN_UP) {
            mod_tz(ctx, MOD_OP_ADD);
            return 1;
        }

        if (event == EVENT_BTN_DOWN) {
            mod_tz(ctx, MOD_OP_SUB);
            return 1;
        }

    }
    return 0;
}

*/

#include "text_label_color.h"
#include "forms_split.h"
#include "widget_text.h"

typedef struct {
    tf_cfg_t title;
} ctx_settings_title_t;

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

static const label_color_t tz_title = { .color = COLOR(0x11ff44), .l = { .text_list = (const char * []){ "TIME", "DATE", "TIME ZONE" }, .xy = { .x = 2 }, .t = LP_T_LIDX } };

static const label_list_t title_selector = {
    .wrap_list = (label_color_t []) {
        { .color = COLOR(0x11ff44), .l = { .xy = { .x =  0 }, .len = 1, .text_list = (const char *[]){ 0, "[" }, .t = LP_T_LIDX } },
        { .color = COLOR(0x11ff44), .l = { .xy = { .x = 12 }, .len = 1, .text_list = (const char *[]){ 0, "]" }, .t = LP_T_LIDX } },
    },
    .count = 2
};

static const bg[] = { COLOR(112233), COLOR(0x556677) };

static const ui_node_desc_t setting = {
    .widget = &widget_text,
    .cfg = &(widget_text_cfg_t){
        .tf_cfg = &title,
        .bg = bg,
        .label = &(label_color_t){
            .color = COLOR(0x11ff89),
            .l = { .t = LP_T, .text = "SETTINGS" }
        }
    }
};

enum {
    TIME_SETTINGS_TIME,
    TIME_SETTINGS_DATE,
    TIME_SETTINGS_TIME_ZONE,
};

typedef struct { unsigned s; time_t t; } uv_time_t;
typedef struct { unsigned days; date_t d; uint8_t dow; } uv_date_t;
typedef struct { int s; time_t t; uint8_t sign; } uv_tz_t;

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

static void ctx_uv_update_time_s(uv_time_t * ctx, unsigned x)
{
    (void)x;
    ctx->s = rtc_get_time_s();
}

static void ctx_uv_update_date(uv_date_t * ctx, unsigned x)
{
    (void)x;
    ctx->days = days_from_s(rtc_get_time_s());
}

// static void update_uv_time(uv_time_t * time, )

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
            { .color = COLOR(0x1144ff), { .t = LP_T, .text = ":", .xy = { .x = -4 }, .len = 1 } },
        },
        .count = 1,
    },
};

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
                    .sl = &(const label_list_t){ .ctx_update = (void(*)(void * ctx, unsigned x))time_from_s, .list = labels_time, .count = 3 }
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
                    .sl = &(const label_list_t){ .ctx_update = (void(*)(void * ctx, unsigned x))date_from_days, .list = labels_date, .count = 3 }
                }
            },
                // { .t = , .xy = { .x = 0  - 15 }, .ofs = offsetof(ctx_t, dow), .vt = { .l = 3, }, .to_str = day_of_week_name },


            // { .color = COLOR(0x1144ff), .l = { .len = 3, .rep = { .vs = VAL_SIZE_32 }, .to_str =  } },
            // { .color = COLOR(0x1144ff), .l = { .len = 2, .rep = { .vs = VAL_SIZE_8 }, .t = LV, .ofs = offsetof(uv_date_t, t.d) } },
            // { .color = COLOR(0x1144ff), .l = { .len = 3, .rep = { .vs = VAL_SIZE_8 }, .t = LF, .to_str = month_name, .ofs = offsetof(uv_date_t, t.m) } },
            // { .color = COLOR(0x1144ff), .l = { .len = 4, .rep = { .vs = VAL_SIZE_16 }, .t = LV, .ofs = offsetof(uv_date_t, t.y) } },


        },
        .count = 1,
        .ctx_update = (void(*)(void *, unsigned))ctx_uv_update_date
    },
    [TIME_SETTINGS_TIME_ZONE] = {}
};



typedef struct {
    uv_mod_t * mod_list;
    void (*prepare)(void * ctx);
    void (*apply)(void * ctx);
    void (*change)(void * ctx);
    uint16_t offset;
    uint8_t count;
} setting_mod_list_t;

#include "dp.h"

void select_val(tf_ctx_t * tf, label_t * l, void * ctx, unsigned select)
{
    const lcd_color_t c[] = { COLOR(0x1144ff), COLOR(0x886612) };
    label_color(tf, &(label_color_t){ .color = c[select], .l = *l }, bg[0], 0, ctx, 0);
}

void mod_prepare_time(ctx_ts_t * ctx)
{
    printf("prepare offset h %d\n", offsetof(time_t, h));
    dpxd(&labels_time[0], 1, sizeof(label_t) ); dn();

    ctx->uv.time.t.s = 0;
    select_val(&ctx->tf, &labels_time[2], &ctx->uv.time.t, 0);
}

void mod_apply_time(ctx_ts_t * ctx)
{
    printf("apply \n");
    int tz = time_zone_get();
    unsigned time_s = rtc_get_time_s() + tz;
    rtc_set_time_s(time_change_in_s(&ctx->uv.time.t, time_s) - tz);
}

static const setting_mod_list_t mod[] = {
    {
        .apply = mod_apply_time,
        .prepare = mod_prepare_time,
        .count = 2,
        .offset = offsetof(ctx_ts_t, uv.time.t),
        .mod_list = (uv_mod_t []) {
            { .cfg = { .u_min = 0,    .u_max = 23,   .u_step = 1, .ovf = 1 }, .l = &labels_time[0] },
            { .cfg = { .u_min = 0,    .u_max = 59,   .u_step = 1, .ovf = 1 }, .l = &labels_time[1] }
        }
    },
    {
        .count = 3,
        .offset = offsetof(ctx_ts_t, uv.date.d),
        .mod_list = (uv_mod_t []) {
            { .cfg = { .u_min = 1,    .u_max = 31,   .u_step = 1, .ovf = 1 }, .l = &labels_date[0] },
            { .cfg = { .u_min = 0,    .u_max = 11,   .u_step = 1, .ovf = 1 }, .l = &labels_date[1] },
            { .cfg = { .u_min = 2000, .u_max = 2100, .u_step = 1, .ovf = 0 }, .l = &labels_date[2] },
        }
    },
    {}
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

    printf("change idx %d state %d\n", el->idx, ctx->state);

    if (ctx->state) {
        void * vp = uv + m->l->ofs;
        label_color_t l = { .color = COLOR(0x1144ff), .l = *m->l };

        if (event == EVENT_BTN_UP) {
            val_mod(vp, m->l->rep, &m->cfg, MOD_OP_ADD);
            printf("time change up %d %d %d\n", ctx->uv.time.t.h, ctx->uv.time.t.m, ctx->uv.time.t.s);
            select_val(&ctx->tf, m->l, uv, 1);
            if (mod[el->idx].change) {
                mod[el->idx].change(ctx);
            }
            return 1;
        }

        if (event == EVENT_BTN_DOWN) {
            val_mod(vp, m->l->rep, &m->cfg, MOD_OP_SUB);
            printf("time change dn %d %d %d\n", ctx->uv.time.t.h, ctx->uv.time.t.m, ctx->uv.time.t.s);
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
            printf("time change prepare %d %d %d\n", ctx->uv.time.t.h, ctx->uv.time.t.m, ctx->uv.time.t.s);
        }
        ctx->state = new_state;
        printf("time change state %d\n", ctx->state);
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
            // printf(" -- setting form %d %d %d %d, text %d %d, pos %d %d\n", el->f.s.x, el->f.s.y, el->f.p.x, el->f.p.y, ctx->title.size.x, ctx->title.size.y, ctx->title.xy.x, ctx->title.xy.y);

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
