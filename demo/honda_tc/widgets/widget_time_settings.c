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
    enum {
        EDIT_NONE,
        EDIT_H,
        EDIT_M,

        EDIT_YEAR,
        EDIT_MONTH,
        EDIT_DAY,

        EDIT_TIMEZONE
    } state;
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
    f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("00:00:00") }, &tcfg);
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
            ctx->state = EDIT_NONE;
            unsigned time_s = time_to_s(&ctx->time);
            unsigned days = days_from_s(time_s);
            time_s = days_to_s(days);
            time_s += time_to_s(&ctx->time);
            rtc_set_time_s(time_s);
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
    ctx->state = EDIT_NONE;

    form_t f;
    f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("Date set:") }, &fcfg);
    form_align(&el->f, &f, &ALIGN_MODE(LI, 8, UI, 8));
    ctx->title_pos = f.p;

    f.s = lcd_text_size_px(&(xy_t){ .x = TEXT_LEN("TUE 21 APR 2000") }, &fcfg);
    form_align(&el->f, &f, &ALIGN_MODE(RI, 8, DI, 8));

    draw_color_form(&f, 0xFFEEDD);
    ctx->text_pos = f.p;
}

static void print_date(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    char str[8];

    lcd_color_text_raw_print(day_of_week_name(day_of_week(&ctx->date)), &fcfg, cs(el->active, ctx->state == EDIT_DAY), &ctx->text_pos, 0, &(xy_t){ .x = 0 }, 3);

    dec_to_str_right_aligned(ctx->date.d, str, 2, 1);
    lcd_color_text_raw_print(str, &fcfg, cs(el->active, ctx->state == EDIT_DAY), &ctx->text_pos, 0, &(xy_t){ .x = 4 }, 2);

    lcd_color_text_raw_print(month_name(ctx->date.m), &fcfg, cs(el->active, ctx->state == EDIT_MONTH), &ctx->text_pos, 0, &(xy_t){ .x = 7 }, 3);

    dec_to_str_right_aligned(ctx->date.y, str, 4, 1);
    lcd_color_text_raw_print(str, &fcfg, cs(el->active, ctx->state == EDIT_YEAR), &ctx->text_pos, 0, &(xy_t){ .x = 11 }, 4);
}

static void update_date(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // printf("update date, current day %d state %d\n", ctx->current_day, ctx->state);
    if (ctx->state == EDIT_NONE) {
        unsigned current_day = days_from_s(rtc_get_time_s());
        if (ctx->current_day != current_day) {
            ctx->current_day = current_day;
            date_from_days(&ctx->date, current_day);
            print_date(el);
        }
    }
}

static void redraw_date_widget(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    draw_color_form(&el->f, cs(el->active, 0)->bg);
    lcd_color_text_raw_print("Date setup", &fcfg, cs(el->active, 0), &ctx->title_pos, 0, 0, 0);
    ctx->current_day = -1;
    update_date(el);
}

static void process_date(ui_element_t * el, unsigned event)
{

}

/*
план на завтра дописать обработку событий

переписать изменения даты чтобы только изменяемая часть перерисовывалась и день недели
переписать тоже с временем

подумать про конфигурацию текстового поля, положение текста, положение подстроки, как сгенерить текст итд ...
*/

const widget_desc_t __widget_time_settings = {
    .draw = redraw_time_widget,
    .calc = calc,
    .extend = extend,
    .select = redraw_time_widget,
    .process_event = process_time,
    .update = update_time,
    .ctx_size = sizeof(ctx_t)
};

const widget_desc_t __widget_date_settings = {
    .calc = calc_date,
    .extend = extend_date,
    .select = redraw_date_widget,
    .draw = redraw_date_widget,
    // .process_event = process_date,
    .update = update_date,
    .ctx_size = sizeof(ctx_t)
};

const widget_desc_t __widget_time_zone_settings = {

};
