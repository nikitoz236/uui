#include "widget_time_settings.h"
#include "lcd_text.h"
#include "lcd_text_color.h"
#include "tc_events.h"
#include "tc_colors.h"
#include "date_time.h"
#include "rtc.h"
#include "align_forms.h"
#include "draw_color.h"
#include "str_val.h"
#include "val_pack.h"
#include "mod_macro.h"

/*
    состояние 1 - отображаем текущее время
        при нажатии вверх вниз выбираются другие виджеты
        при нажатии кнопки ок, выделяются часы, секунды становятся 00, состояние 2

    состояние 2 - меняем часы
        кнопки вверх вниз меняют часы
        при нажатии кнопки ок, выделяются минуты, состояние 3
        кнопка ок переходит к минутам
        кнопка назад выходит из редактирования
        кнопка долгое назад тоже выходит из редактирования, состояние 1

    состояние 3 - меняем минуты
        кнопки вверх вниз меняют минуты
        кнопка назад переходит к часам, состояние 2
        кнопка ок устанавливает время
        кнопка долгое назад тоже выходит из редактирования, состояние 1
*/

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
            VTU_NONE
        } vtu;
        enum {
            EDIT_NONE,
            EDIT_H,
            EDIT_M,

            EDIT_YEAR,
            EDIT_MONTH,
            EDIT_DAY,

            EDIT_TIMEZONE
        } state;
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

lcd_text_cfg_t tcfg = {
    .font = &font_5x7,
    .text_size = { .x = 8, .y = 1 },
    .gaps = { .x = 2 , .y = 4 },
    .scale = 2
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

void calc(ui_element_t * el)
{
    el->f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("Time settings"), .y = 2 }, &fcfg);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->state = EDIT_NONE;

    form_t f = el->f;
    f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("00:00:00") }, &fcfg);
    lcd_text_calc_size(&f.s, &tcfg);

    form_align(&el->f, &f, &ALIGN_MODE(LI, tcfg.scale * 4, UI, tcfg.gaps.y));
    ctx->title_pos = f.p;

    form_align(&el->f, &f, &ALIGN_MODE(RI, tcfg.scale * 4, DI, tcfg.gaps.y));
    ctx->text_pos = f.p;
}

static void print_time(ctx_t * ctx, lcd_color_t bg)
{
    char str[4];

    color_scheme_t cs = {
        .bg = bg,
        .fg = tc_colors[TC_COLOR_FG_UNSELECTED]
    };

    dec_to_str_right_aligned(ctx->time.s, str, 2, 1);
    lcd_text_color_print(str, &ctx->text_pos, &tcfg, &cs, 6, 0, 2);

    if (ctx->state == EDIT_H) {
        cs.fg = tc_colors[TC_COLOR_FG_SELECTED];
    } else {
        cs.fg = tc_colors[TC_COLOR_FG_UNSELECTED];
    }

    dec_to_str_right_aligned(ctx->time.h, str, 2, 1);
    lcd_text_color_print(str, &ctx->text_pos, &tcfg, &cs, 0, 0, 2);

    if (ctx->state == EDIT_M) {
        cs.fg = tc_colors[TC_COLOR_FG_SELECTED];
    } else {
        cs.fg = tc_colors[TC_COLOR_FG_UNSELECTED];
    }

    dec_to_str_right_aligned(ctx->time.m, str, 2, 1);
    lcd_text_color_print(str, &ctx->text_pos, &tcfg, &cs, 3, 0, 2);
}

static void update_time(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    if (ctx->state == EDIT_NONE) {
        unsigned current_time_s = rtc_get_time_s();
        if (ctx->current_time_s != current_time_s) {
            ctx->current_time_s = current_time_s;
            time_from_s(&ctx->time, current_time_s);
            lcd_color_t bg = tc_colors[TC_COLOR_BG_UNSELECTED];
            if (el->active) {
                bg = tc_colors[TC_COLOR_BG_SELECTED];
            }
            print_time(ctx, bg);
        }
    }
}

static void redraw_time_widget(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    lcd_color_t bg = tc_colors[TC_COLOR_BG_UNSELECTED];

    if (el->active) {
        bg = tc_colors[TC_COLOR_BG_SELECTED];
    }

    printf("redraw_time_widget, active %d\n", el->active);

    draw_color_form(&el->f, bg);

    lcd_text_color_print("Time setup", &ctx->title_pos, &tcfg, &(color_scheme_t){ .bg = bg, .fg = tc_colors[TC_COLOR_FG_UNSELECTED] }, 0, 0, 0);

    lcd_text_color_print(":", &ctx->text_pos, &tcfg, &(color_scheme_t){ .bg = bg, .fg = tc_colors[TC_COLOR_FG_UNSELECTED] }, 2, 0, 1);
    lcd_text_color_print(":", &ctx->text_pos, &tcfg, &(color_scheme_t){ .bg = bg, .fg = tc_colors[TC_COLOR_FG_UNSELECTED] }, 5, 0, 1);

    ctx->current_time_s = -1;
    update_time(el);
}

static unsigned process_time(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    printf("process_time %d\n", event);

    if (ctx->state == EDIT_NONE) {
        if (event == EVENT_BTN_OK) {
            ctx->state = EDIT_H;
            ctx->time.s = 0;
            print_time(ctx, tc_colors[TC_COLOR_BG_SELECTED]);
            return 1;
        }
        return 0;
    } else if (ctx->state == EDIT_H) {
        if (event == EVENT_BTN_OK) {
            ctx->state = EDIT_M;
            print_time(ctx, tc_colors[TC_COLOR_BG_SELECTED]);
            return 1;
        } else if (event == EVENT_BTN_LEFT) {
            ctx->state = EDIT_NONE;
            ctx->current_time_s = -1;
            update_time(el);
            return 1;
        } else if (event == EVENT_BTN_UP) {
            ctx->time.h++;
            if (ctx->time.h == 24) {
                ctx->time.h = 0;
            }
            print_time(ctx, tc_colors[TC_COLOR_BG_SELECTED]);
            return 1;
        } else if (event == EVENT_BTN_DOWN) {
            if (ctx->time.h == 0) {
                ctx->time.h += 24;
            }
            ctx->time.h--;
            print_time(ctx, tc_colors[TC_COLOR_BG_SELECTED]);
            return 1;
        }
        return 0;
    } else if (ctx->state == EDIT_M) {
        if (event == EVENT_BTN_OK) {
            rtc_set_time_s(time_change_in_s(&ctx->time, rtc_get_time_s()));
            ctx->state = EDIT_NONE;
            ctx->current_time_s = -1;
            update_time(el);
            return 1;
        } else if (event == EVENT_BTN_LEFT) {
            ctx->state = EDIT_H;
            print_time(ctx, tc_colors[TC_COLOR_BG_SELECTED]);
            return 1;
        } else if (event == EVENT_BTN_UP) {
            ctx->time.m++;
            if (ctx->time.m == 60) {
                ctx->time.m = 0;
            }
            print_time(ctx, tc_colors[TC_COLOR_BG_SELECTED]);
            return 1;
        } else if (event == EVENT_BTN_DOWN) {
            if (ctx->time.m == 0) {
                ctx->time.m += 60;
            }
            ctx->time.m--;
            print_time(ctx, tc_colors[TC_COLOR_BG_SELECTED]);
            return 1;
        }
        return 0;
    }
}



//------------------------------------------------------------------------------


#include "val_text.h"
#include "str_val_buf.h"
#include "val_mod.h"

typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t limit_char;
    xy_t margin;
} text_field_cfg_t;

typedef struct {
    const text_field_cfg_t * tfcfg;
    xy_t pos_char;
    val_text_t vt;
    uint16_t min;
    uint16_t max;
    uint8_t ovf : 1;
} val_text_updatable_t;

const text_field_cfg_t tf = {
    .fcfg = &fcfg,
    .limit_char = { .x = TEXT_LEN("TUE 21 APR 2000"), .y = 1 },
    .margin = { .x = 0, .y = 0 }
};


const val_text_updatable_t vtu_list[] = {
    [VTU_YEAR]  = { .tfcfg =  &tf, .pos_char = { .x = 11 }, .vt = { .l = 4, .vs = VAL_SIZE_16, .zl = 0}, .min = 2000, .max = 2100, .ovf = 0 },
    [VTU_DAY]   = { .tfcfg =  &tf, .pos_char = { .x = 4  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 0}, .min = 1, .max = 31, .ovf = 1 },
    [VTU_MONTH] = { .tfcfg =  &tf, .pos_char = { .x = 7  }, .vt = { .l = 3, .vs = VAL_SIZE_8,  .zl = 0}, .min = 0, .max = 12, .ovf = 1 },
    [VTU_H]     = { .tfcfg =  &tf, .pos_char = { .x = 0  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 0}, .min = 0, .max = 23, .ovf = 1 },
    [VTU_M]     = { .tfcfg =  &tf, .pos_char = { .x = 3  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 1}, .min = 0, .max = 59, .ovf = 1 },
    [VTU_S]     = { .tfcfg =  &tf, .pos_char = { .x = 6  }, .vt = { .l = 2, .vs = VAL_SIZE_8,  .zl = 1}, .min = 0, .max = 59, .ovf = 1 },
    [VTU_DOW]   = { .tfcfg =  &tf, .pos_char = { .x = 0  }, .vt = { .l = 3, .vs = VAL_SIZE_8 }, },
};

// хочется скомпактить функции process, для этого нам надо меняльные структуры с мин макс значением, шагом итд.

/*
переписать тоже с временем
*/



static void calc_date(ui_element_t * el)
{
    //  Date setup
    //  TUE 21 APR 2000
    //  0123456789012345

    el->f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("TUE 21 APR 2000"), .y = 2 }, &fcfg);

    // margin
    el->f.s.h += 2 * 8;
    el->f.s.w += 2 * 8;
}

static void extend_date(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->vtu = VTU_NONE;

    form_t f;
    f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("Date set:") }, &fcfg);
    form_align(&el->f, &f, &ALIGN_MODE(LI, 8, UI, 8));
    ctx->title_pos = f.p;

    f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("TUE 21 APR 2000") }, &fcfg);
    form_align(&el->f, &f, &ALIGN_MODE(RI, 8, DI, 8));

    draw_color_form(&f, 0xFFEEDD);
    ctx->text_pos = f.p;
}

static void update_vt(xy_t * pos_px, unsigned vt_idx, void * ptr, color_scheme_t * cs)
{
    const char * str;
    val_text_updatable_t * vtu = &vtu_list[vt_idx];

    if (vt_idx == VTU_DOW) {
        str = day_of_week_name(*(week_day_t *)ptr);
    } else if (vt_idx == VTU_MONTH) {
        str = month_name(*(month_t *)ptr);
    } else {
        str = str_val_buf_get();
        str_val_buf_lock();
        val_text_to_str(str, ptr, &vtu->vt);
        // printf("update vt val_text_to_str %p\n", str);
    }

    printf("update vt %d %s cs: %06X %06X, pos_char: %d %d len %d, pos px %d %d\n", vt_idx, str, cs->bg, cs->fg, vtu->pos_char.x, vtu->pos_char.y, vtu->vt.l, pos_px->x, pos_px->y);

    lcd_color_text_raw_print(str, vtu->tfcfg->fcfg, cs, pos_px, &vtu->tfcfg->limit_char, &vtu->pos_char, vtu->vt.l);
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
    lcd_color_text_raw_print("Date setup", &fcfg, cs(el->active, 0), &ctx->title_pos, 0, 0, 0);
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

    if (ctx->vtu == VTU_NONE) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_YEAR;
            update_vt(&ctx->text_pos, VTU_YEAR, &ctx->date.y, cs(1, 1));
            return 1;
        }
    } else if (ctx->vtu == VTU_YEAR) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_MONTH;
            update_vt(&ctx->text_pos, VTU_YEAR, &ctx->date.y, cs(1, 0));
            update_vt(&ctx->text_pos, VTU_MONTH, &ctx->date.m, cs(1, 1));
            return 1;
        } else if (event == EVENT_BTN_LEFT) {
            ctx->vtu = VTU_NONE;
            ctx->current_day = -1;
            update_date(el);
            return 1;
        } else if (event == EVENT_BTN_UP) {
            mod = MOD_UP;
        } else if (event == EVENT_BTN_DOWN) {
            mod = MOD_DOWN;
        }
        if (mod != MOD_NONE) {
            if (val_mod_unsigned(&ctx->date.y, vtu_list[VTU_YEAR].vt.vs, mod, 0, 2000, 2100, 1)) {
                update_vt_and_dow(&ctx->text_pos, &ctx->date, &ctx->date.y, VTU_YEAR, 1, 1);
            }
            return 1;
        }
    } else if (ctx->vtu == VTU_MONTH) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_DAY;
            update_vt(&ctx->text_pos, VTU_MONTH, &ctx->date.m, cs(1, 0));
            update_vt(&ctx->text_pos, VTU_DAY, &ctx->date.d, cs(1, 1));
            return 1;
        } else if (event == EVENT_BTN_LEFT) {
            ctx->vtu = VTU_YEAR;
            update_vt(&ctx->text_pos, VTU_MONTH, &ctx->date.m, cs(1, 0));
            update_vt(&ctx->text_pos, VTU_YEAR, &ctx->date.y, cs(1, 1));
            return 1;
        } else if (event == EVENT_BTN_UP) {
            ctx->date.m++;
            if (ctx->date.m == 12) {
                ctx->date.m = 0;
            }
            update_vt_and_dow(&ctx->text_pos, &ctx->date, &ctx->date.m, VTU_MONTH, 1, 1);
            return 1;
        } else if (event == EVENT_BTN_DOWN) {
            if (ctx->date.m == 0) {
                ctx->date.m = 12;
            }
            ctx->date.m--;
            update_vt_and_dow(&ctx->text_pos, &ctx->date, &ctx->date.m, VTU_MONTH, 1, 1);
            return 1;
        }
    } else if (ctx->vtu == VTU_DAY) {
        if (event == EVENT_BTN_OK) {
            ctx->vtu = VTU_NONE;
            rtc_set_time_s(date_change_in_s(&ctx->date, rtc_get_time_s()));
            update_date(el);
            return 1;
        } else if (event == EVENT_BTN_LEFT) {
            ctx->vtu = VTU_MONTH;
            update_vt(&ctx->text_pos, VTU_DAY, &ctx->date.d, cs(1, 0));
            update_vt(&ctx->text_pos, VTU_MONTH, &ctx->date.m, cs(1, 1));
            return 1;
        } else if (event == EVENT_BTN_UP) {
            MOD_OVF_ADD(ctx->date.d, 1 + days_in_month(ctx->date.m, ctx->date.y), 1);
            update_vt_and_dow(&ctx->text_pos, &ctx->date, &ctx->date.d, VTU_DAY, 1, 1);
            return 1;
        } else if (event == EVENT_BTN_DOWN) {
            if (ctx->date.d == 1) {
                ctx->date.d = days_in_month(ctx->date.m, ctx->date.y) + 1;
            }
            ctx->date.d--;
            update_vt_and_dow(&ctx->text_pos, &ctx->date, &ctx->date.d, VTU_DAY, 1, 1);
            return 1;
        }
    }
    return 0;
}

const widget_desc_t __widget_time_settings = {
    .calc = calc,
    .extend = extend,
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

};
