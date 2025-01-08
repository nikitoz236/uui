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

#include "val_text.h"
#include "str_val_buf.h"
#include "val_mod.h"
#include "forms_align.h"
#include "stddef.h"

#include "time_zone.h"

typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t limit_char;
    xy_t margin;
} text_field_cfg_t;

typedef struct {
    const text_field_cfg_t * tfcfg;
    unsigned offset_in_ctx;
    const char * (*to_str)(unsigned x);
    uint16_t min;
    uint16_t max;
    uint16_t step;
    xy_t pos_char;
    val_text_t vt;
    uint8_t ovf : 1;
} val_text_updatable_t;

typedef struct {
    xy_t text_pos;
    xy_t title_pos;
    union {
        struct {
            unsigned current_time_s;
            time_t time;
        };
        struct {
            unsigned current_day;
            date_t date;
        };
        struct {
            int tz_s;
            time_t tz_time;
            uint8_t tz_sign;
        };
    };
    enum {
        VTU_YEAR = 0,
        VTU_MONTH = 1,
        VTU_DAY = 2,
        VTU_H = 0,
        VTU_M = 1,
        VTU_S = 2,
        VTU_TZ_SIGN = 2,
        VTU_DOW = 3,
        VTU_NONE,
    } vtu;
    uint8_t dow;
} ctx_t;

extern font_t font_5x7;

const lcd_font_cfg_t fcfg = {
    .font = &font_5x7,
    .gaps = { .x = 2, .y = 8 },
    .scale = 2
};

const xy_t text_margin = { .x = 8, .y = 8 };

const text_field_cfg_t tf = {
    .fcfg = &fcfg,
    .limit_char = { .x = TEXT_LEN("TUE 21 APR 2000"), .y = 1 },
    .margin = { .x = 0, .y = 0 }
};

const char * tz_sign_to_str(unsigned sign)
{
    if (sign) {
        return "-";
    }
    return "+";
}

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


static inline color_scheme_t * cs(unsigned is_active, unsigned is_edit)
{
    static color_scheme_t color_scheme;

    if (is_active) {
        color_scheme.bg = tc_colors[TC_COLOR_BG_SELECTED];
    } else {
        color_scheme.bg = tc_colors[TC_COLOR_BG_UNSELECTED];
    }
    if (is_edit) {
        color_scheme.fg = tc_colors[TC_COLOR_FG_SELECTED];
    } else {
        color_scheme.fg = tc_colors[TC_COLOR_FG_UNSELECTED];
    }
    return &color_scheme;
}

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
        str_val_buf_lock();
        val_text_to_str((char*)str, ptr, &vtu->vt);
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

xy_t size_add_margins(xy_t size, xy_t margins)
{
    return (xy_t){
        .x = size.x + margins.x * 2,
        .y = size.y + margins.y * 2
    };
}

static xy_t calc_setting_form_size(unsigned len)
{
    return size_add_margins(lcd_text_size_px(&(xy_t){ .x = len, .y = 2 }, &fcfg), text_margin);
}

static void calc_pos(ctx_t * ctx, form_t * f, unsigned title_len, unsigned text_len)
{
    ctx->title_pos = align_form_pos(f, lcd_text_size_px(&(xy_t){ .x = title_len, .y = 1 }, &fcfg), &(align_t){ .x = { .edge = EDGE_L }, .y = { .edge = EDGE_U } }, &text_margin);
    ctx->text_pos = align_form_pos(f, lcd_text_size_px(&(xy_t){ .x = text_len, .y = 1 }, &fcfg), &(align_t){ .x = { .edge = EDGE_R }, .y = { .edge = EDGE_D } }, &text_margin);
}

static void calc_time(ui_element_t * el)
{
    el->f.s = calc_setting_form_size(TEXT_LEN("Time set:"));
}

static void extend_time(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->vtu = VTU_NONE;
    calc_pos(ctx, &el->f, TEXT_LEN("Time set:"), TEXT_LEN("00:00:00"));
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

static void calc_tz(ui_element_t * el)
{
    //  Time zone:
    //  -12:00
    //  0123456789012345
    el->f.s = calc_setting_form_size(TEXT_LEN("Time zone:"));
}

static void extend_tz(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->vtu = VTU_NONE;
    calc_pos(ctx, &el->f, TEXT_LEN("Time zone:"), TEXT_LEN("- 12:00"));
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

const widget_desc_t __widget_time_settings = {
    .calc = calc_time,
    .extend = extend_time,
    .select = redraw_time_widget,
    .draw = redraw_time_widget,
    .process_event = process_time,
    .update = update_time,
    .ctx_size = sizeof(ctx_t)
};

const widget_desc_t __widget_date_settings = {
    .calc = calc_date,
    .extend = extend_date,
    .select = redraw_date_widget,
    .draw = redraw_date_widget,
    .process_event = process_date,
    .update = update_date,
    .ctx_size = sizeof(ctx_t)
};

const widget_desc_t __widget_time_zone_settings = {
    .calc = calc_tz,
    .extend = extend_tz,
    .select = redraw_tz_widget,
    .draw = redraw_tz_widget,
    .process_event = process_tz,
    .ctx_size = sizeof(ctx_t)
};
