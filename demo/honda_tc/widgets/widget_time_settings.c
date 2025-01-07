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

typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t limit_char;
    xy_t margin;
} text_field_cfg_t;

typedef struct {
    const text_field_cfg_t * tfcfg;
    uint16_t min;
    uint16_t max;
    uint16_t step;
    xy_t pos_char;
    val_text_t vt;
    uint8_t ovf : 1;
} val_text_updatable_t;

typedef struct {
    union {
        enum {
            VTU_DAY,
            VTU_MONTH,
            VTU_YEAR,
            VTU_DOW,
            VTU_H,
            VTU_M,
            VTU_S,
            VTU_TZ,
            VTU_NONE
        } vtu;
    };
    union {
        struct {
            time_t time;
            unsigned current_time_s;
        };
        struct {
            date_t date;
            unsigned current_day;
        };
    };
    xy_t text_pos;
    xy_t title_pos;
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

const val_text_updatable_t vtu_list[] = {
    [VTU_YEAR]  = { .tfcfg =  &tf, .pos_char = { .x = 11 }, .vt = { .l = 4, .vs = VAL_SIZE_16, .zl = 0}, .min = 2000, .max = 2100, .step = 1, .ovf = 0 },
    [VTU_DAY]   = { .tfcfg =  &tf, .pos_char = { .x = 4  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 0}, .min = 1,    .max = 31,   .step = 1, .ovf = 1 },
    [VTU_MONTH] = { .tfcfg =  &tf, .pos_char = { .x = 7  }, .vt = { .l = 3, .vs = VAL_SIZE_8,  .zl = 0}, .min = 0,    .max = 11,   .step = 1, .ovf = 1 },
    [VTU_H]     = { .tfcfg =  &tf, .pos_char = { .x = 0  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 0}, .min = 0,    .max = 23,   .step = 1, .ovf = 1 },
    [VTU_M]     = { .tfcfg =  &tf, .pos_char = { .x = 3  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 1}, .min = 0,    .max = 59,   .step = 1, .ovf = 1 },
    [VTU_S]     = { .tfcfg =  &tf, .pos_char = { .x = 6  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 1}, },
    [VTU_DOW]   = { .tfcfg =  &tf, .pos_char = { .x = 0  }, .vt = { .l = 3, .vs = VAL_SIZE_8, }, },
};

static void update_vt(xy_t * pos_px, unsigned vt_idx, void * ptr, color_scheme_t * cs)
{
    const char * str;
    const val_text_updatable_t * vtu = &vtu_list[vt_idx];

    if (vt_idx == VTU_DOW) {
        str = day_of_week_name(*(week_day_t *)ptr);
    } else if (vt_idx == VTU_MONTH) {
        str = month_name(*(month_t *)ptr);
        // printf("update vt month %s\n", str);
    } else {
        str = str_val_buf_get();
        str_val_buf_lock();
        val_text_to_str((char*)str, ptr, &vtu->vt);
        // printf("update vt val_text_to_str %p\n", str);
    }

    // printf("update vt %d %s cs: %06X %06X, pos_char: %d %d len %d, pos px %d %d\n", vt_idx, str, cs->bg, cs->fg, vtu->pos_char.x, vtu->pos_char.y, vtu->vt.l, pos_px->x, pos_px->y);

    lcd_color_text_raw_print(str, vtu->tfcfg->fcfg, cs, pos_px, &vtu->tfcfg->limit_char, &vtu->pos_char, vtu->vt.l);
}

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
        unsigned current_time_s = rtc_get_time_s();
        if (ctx->current_time_s != current_time_s) {
            ctx->current_time_s = current_time_s;
            time_from_s(&ctx->time, current_time_s);

            update_vt(&ctx->text_pos, VTU_H, &ctx->time.h, cs(el->active, 0));
            update_vt(&ctx->text_pos, VTU_M, &ctx->time.m, cs(el->active, 0));
            update_vt(&ctx->text_pos, VTU_S, &ctx->time.s, cs(el->active, 0));
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

static void * ptr_of_time(time_t * t, unsigned id)
{
    switch (id) {
        case VTU_H: return &t->h;
        case VTU_M: return &t->m;
    }
    return 0;
};

static unsigned process_time(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("process_time %d\n", event);

    enum {
        MOD_UP = MOD_OP_ADD,
        MOD_DOWN = MOD_OP_SUB,
        MOD_NONE
    } mod = MOD_NONE;

    unsigned change_vtu = ctx->vtu;

    if (ctx->vtu == VTU_NONE) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_H;
            ctx->time.s = 0;
            update_vt(&ctx->text_pos, VTU_H, &ctx->time.h, cs(1, 1));
            update_vt(&ctx->text_pos, VTU_S, &ctx->time.s, cs(1, 0));
            return 1;
        }
    } else {
        if (event == EVENT_BTN_OK) {
            if (ctx->vtu == VTU_H) {
                change_vtu = VTU_M;
            } else if (ctx->vtu == VTU_M) {
                change_vtu = VTU_NONE;
            }
            if (change_vtu == VTU_NONE) {
                // set time
                rtc_set_time_s(time_change_in_s(&ctx->time, rtc_get_time_s()));
            }
        }

        if (event == EVENT_BTN_LEFT) {
            // static unsigned next_vtu_list[] = {
            //     [VTU_H] = VTU_NONE,
            //     [VTU_M] = VTU_H,
            // };
            // change_vtu = next_vtu_list[ctx->vtu];
            // изза того что VTU_H не начинается с 0, требуется заполнять предыдущий массив нулями
            // компилятору для этого нужен memset
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
                update_vt(&ctx->text_pos, ctx->vtu, ptr_of_time(&ctx->time, ctx->vtu), cs(1, 0));
                ctx->vtu = change_vtu;
                update_vt(&ctx->text_pos, ctx->vtu, ptr_of_time(&ctx->time, ctx->vtu), cs(1, 1));
            }
            return 1;
        }

        if (event == EVENT_BTN_UP) {
            mod = MOD_UP;
        }

        if (event == EVENT_BTN_DOWN) {
            mod = MOD_DOWN;
        }

        if (mod != MOD_NONE) {
            const val_text_updatable_t * vtu = &vtu_list[ctx->vtu];
            if (val_mod_unsigned(ptr_of_time(&ctx->time, ctx->vtu), vtu->vt.vs, mod, vtu->ovf, vtu->min, vtu->max, vtu->step)) {
                update_vt(&ctx->text_pos, ctx->vtu, ptr_of_time(&ctx->time, ctx->vtu), cs(1, 1));
            }
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

static void update_vt_and_dow(xy_t * pos_px, date_t * d, void * ptr, unsigned vt_idx, unsigned active, unsigned edit)
{
    update_vt(pos_px, vt_idx, ptr, cs(active, edit));
    week_day_t dow = day_of_week(d);
    update_vt(pos_px, VTU_DOW, &dow, cs(active, 0));
}

static void update_date(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("update date, current day %d vtu %d\n", ctx->current_day, ctx->vtu);
    if (ctx->vtu == VTU_NONE) {
        unsigned current_day = days_from_s(rtc_get_time_s());
        if (ctx->current_day != current_day) {
            ctx->current_day = current_day;
            date_from_days(&ctx->date, current_day);

            update_vt(&ctx->text_pos, VTU_DAY, &ctx->date.d, cs(el->active, 0));
            update_vt(&ctx->text_pos, VTU_MONTH, &ctx->date.m, cs(el->active, 0));
            update_vt_and_dow(&ctx->text_pos, &ctx->date, &ctx->date.y, VTU_YEAR, el->active, 0);
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
    update_date(el);
}

static unsigned process_date(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("process date %d\n", event);

    enum {
        MOD_UP = MOD_OP_ADD,
        MOD_DOWN = MOD_OP_SUB,
        MOD_NONE
    } mod = MOD_NONE;

    unsigned change_vtu = ctx->vtu;

    if (ctx->vtu == VTU_NONE) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_YEAR;
            update_vt(&ctx->text_pos, VTU_YEAR, &ctx->date.y, cs(1, 1));
            return 1;
        }
    } else {
        void * ptrs[] = {
            [VTU_YEAR] = &ctx->date.y,
            [VTU_MONTH] = &ctx->date.m,
            [VTU_DAY] = &ctx->date.d
        };

        if (event == EVENT_BTN_OK) {
            unsigned next_vtu_list[] = {
                [VTU_YEAR] = VTU_MONTH,
                [VTU_MONTH] = VTU_DAY,
                [VTU_DAY] = VTU_NONE
            };
            change_vtu = next_vtu_list[ctx->vtu];
            if (change_vtu == VTU_NONE) {
                // set date
                rtc_set_time_s(date_change_in_s(&ctx->date, rtc_get_time_s()));
            }
        }

        if (event == EVENT_BTN_LEFT) {
            static unsigned next_vtu_list[] = {
                [VTU_YEAR] = VTU_NONE,
                [VTU_MONTH] = VTU_YEAR,
                [VTU_DAY] = VTU_MONTH
            };
            change_vtu = next_vtu_list[ctx->vtu];
            if (change_vtu == VTU_NONE) {
                ctx->current_day = -1;
            }
        }

        if (change_vtu != ctx->vtu) {
            if (change_vtu == VTU_NONE) {
                ctx->vtu = VTU_NONE;
                update_date(el);
            } else {
                update_vt(&ctx->text_pos, ctx->vtu, ptrs[ctx->vtu], cs(1, 0));
                ctx->vtu = change_vtu;
                update_vt(&ctx->text_pos, ctx->vtu, ptrs[ctx->vtu], cs(1, 1));
            }
            return 1;
        }

        if (event == EVENT_BTN_UP) {
            mod = MOD_UP;
        }

        if (event == EVENT_BTN_DOWN) {
            mod = MOD_DOWN;
        }
        if (mod != MOD_NONE) {
            const val_text_updatable_t * vtu = &vtu_list[ctx->vtu];
            if (val_mod_unsigned(ptrs[ctx->vtu], vtu->vt.vs, mod, vtu->ovf, vtu->min, vtu->max, vtu->step)) {
                update_vt_and_dow(&ctx->text_pos, &ctx->date, ptrs[ctx->vtu], ctx->vtu, 1, 1);
            }
            return 1;
        }
    }
    return 0;
}

static void calc_tz(ui_element_t * el)
{
    xy_t ts = size_add_margins(lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("Time zone:"), .y = 2 }, &fcfg), text_margin);
}

static void extend_tz(ui_element_t * el)
{
    // ctx_t * ctx = (ctx_t *)el->ctx;
    // ctx->title_pos = align_form_pos(&el->f, &lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("Time zone:"), .y = 2 }, &fcfg), &(align_t){ .x = { .edge = EDGE_L }, .y = { .edge = EDGE_U } }, &text_margin);
    // ctx->text_pos = align_form_pos(&el->f, &lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("+12:45"), .y = 2 }, &fcfg), &(align_t){ .x = { .edge = EDGE_L }, .y = { .edge = EDGE_U } }, &text_margin);
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
    // .calc = calc_tz,
    // .extend = extend_tz,
    // .select = redraw_tz_widget,
    // .draw = redraw_tz_widget,
    // .process_event = process_tz
};
