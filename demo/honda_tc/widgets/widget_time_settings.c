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
        EDIT_M
    } state;
    time_t time;
    unsigned current_time_s;
    xy_t text_pos;
} time_settings_ctx_t;

extern font_t font_5x7;

lcd_text_cfg_t tcfg = {
    .font = &font_5x7,
    .text_size = { .x = 8, .y = 1 },
    .gaps = { .x = 2 , .y = 4 },
    .scale = 2
};

void calc(ui_element_t * el)
{
    lcd_text_calc_size(&el->f.s, &tcfg);
    el->f.s.h += 2 * tcfg.gaps.y;
}

static void print_time(time_settings_ctx_t * ctx, lcd_color_t bg)
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

static void update(ui_element_t * el)
{
    time_settings_ctx_t * ctx = (time_settings_ctx_t *)el->ctx;
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

static void redraw_widget(ui_element_t * el)
{
    time_settings_ctx_t * ctx = (time_settings_ctx_t *)el->ctx;

    lcd_color_t bg = tc_colors[TC_COLOR_BG_UNSELECTED];

    if (el->active) {
        bg = tc_colors[TC_COLOR_BG_SELECTED];
    }

    draw_color_form(&el->f, bg);
    lcd_text_color_print(":", &ctx->text_pos, &tcfg, &(color_scheme_t){ .bg = bg, .fg = tc_colors[TC_COLOR_FG_UNSELECTED] }, 2, 0, 1);
    lcd_text_color_print(":", &ctx->text_pos, &tcfg, &(color_scheme_t){ .bg = bg, .fg = tc_colors[TC_COLOR_FG_UNSELECTED] }, 5, 0, 1);

    ctx->current_time_s = -1;
    update(el);
}

static void draw(ui_element_t * el)
{
    time_settings_ctx_t * ctx = (time_settings_ctx_t *)el->ctx;
    ctx->state = EDIT_NONE;

    printf("draw time settings form size %d %d\r\n", el->f.s.w, el->f.s.h);

    form_t f = el->f;
    lcd_text_calc_size(&f.s, &tcfg);
    form_align(&el->f, &f, &ALIGN_MODE(LI, tcfg.gaps.x, C, 0));
    ctx->text_pos = f.p;

    redraw_widget(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    if (el->active == 0) {
        return 0;
    }

    time_settings_ctx_t * ctx = (time_settings_ctx_t *)el->ctx;

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
            update(el);
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
            update(el);
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

widget_desc_t __widget_time_settings = {
    .draw = draw,
    .calc = calc,
    .select = redraw_widget,
    .process_event = process,
    .update = update,
    .ctx_size = sizeof(time_settings_ctx_t)
};
