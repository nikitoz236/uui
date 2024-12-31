#include "widget_watch_screen.h"

#include "align_forms.h"
#include "forms_split.h"
#include "draw_color.h"
#include "lcd_text_color.h"

#include "rtc.h"
#include "date_time.h"
#include "str_val.h"

enum object_list {
    O_TIME,
    O_DATE,
    O_VOLTAGE,
    O_TEMPERATURE,

    O_COUNT
};

#define DATE_TEXT_LEN       TEXT_LEN("21 APR 1990")

extern font_t font_3x5;
extern font_t font_5x7;
lcd_text_cfg_t text_cfg[] = {
    [O_TIME] = {
        .font = &font_3x5,
        .text_size = {5, 1},
        .gaps = { .x = 8 },
        .scale = 16
    },
    [O_DATE] = {
        .font = &font_5x7,
        .text_size = {DATE_TEXT_LEN, 1},
        .gaps = { .x = 3},
        .scale = 3
    },
    [O_VOLTAGE] = {
        .font = &font_5x7,
        .text_size = {6, 1},
        .gaps = {.x = 2},
        .scale = 2
    },
    [O_TEMPERATURE] = {
        .font = &font_5x7,
        .text_size = {8, 1},
        .gaps = {.x = 2},
        .scale = 2
    }
};

typedef struct {
    form_t forms[O_COUNT];
    unsigned last_time_s;
    unsigned last_day;

} watch_screen_ctx_t;

color_scheme_t cs = {
    .bg = 0x000000,
    .fg = 0xffc400
};

void update(ui_element_t * el)
{
    watch_screen_ctx_t * ctx = (watch_screen_ctx_t *)el->ctx;

    char str[16];

    unsigned time_s = rtc_get_time_s();

    if (ctx->last_time_s != time_s) {
        ctx->last_time_s = time_s;

        time_t t;
        time_from_s(&t, time_s);

        dec_to_str_right_aligned(t.h, str, 2, 0);
        lcd_text_color_print(str, &ctx->forms[O_TIME].p, &text_cfg[O_TIME], &cs, 0, 0, 2);
        dec_to_str_right_aligned(t.m, str, 2, 1);
        lcd_text_color_print(str, &ctx->forms[O_TIME].p, &text_cfg[O_TIME], &cs, 3, 0, 2);
    }

    unsigned days = days_from_s(time_s);

    if (ctx->last_day != days) {
        ctx->last_day = days;

        date_t d;
        date_from_days(&d, days);

        date_dd_mname_yyyy_to_str(&d, str);
        lcd_text_color_print(str, &ctx->forms[O_DATE].p, &text_cfg[O_DATE], &cs, 0, 0, DATE_TEXT_LEN);
    }

    lcd_text_color_print("14.7 V", &ctx->forms[O_VOLTAGE].p, &text_cfg[O_VOLTAGE], &cs, 0, 0, 0);
    lcd_text_color_print("-12.6 *C", &ctx->forms[O_TEMPERATURE].p, &text_cfg[O_TEMPERATURE], &cs, 0, 0, 0);
}

void calc_form_for_text(form_t * pf, form_t * f, lcd_text_cfg_t * cfg)
{
    lcd_text_calc_size(&f->s, cfg);
    form_align(pf, f, &ALIGN_MODE(C, 0, C, 0));
}

void draw(ui_element_t * el)
{
    watch_screen_ctx_t * ctx = (watch_screen_ctx_t *)el->ctx;
    ctx->last_day = -1;
    ctx->last_time_s = -1;

    form_t tmp;

    tmp = form_split_half(&el->f, DIMENSION_Y, EDGE_U);
    calc_form_for_text(&tmp, &ctx->forms[O_TIME], &text_cfg[O_TIME]);

    form_t g1 = form_split_half(&el->f, DIMENSION_Y, EDGE_D);
    tmp = form_split_half(&g1, DIMENSION_Y, EDGE_U);
    calc_form_for_text(&tmp, &ctx->forms[O_DATE], &text_cfg[O_DATE]);

    form_t g2 = form_split_half(&g1, DIMENSION_Y, EDGE_D);
    tmp = form_split_half(&g2, DIMENSION_X, EDGE_L);
    calc_form_for_text(&tmp, &ctx->forms[O_VOLTAGE], &text_cfg[O_VOLTAGE]);

    tmp = form_split_half(&g2, DIMENSION_X, EDGE_R);
    calc_form_for_text(&tmp, &ctx->forms[O_TEMPERATURE], &text_cfg[O_TEMPERATURE]);

    draw_color_form(&el->f, cs.bg);
    lcd_text_color_print(":", &ctx->forms[O_TIME].p, &text_cfg[O_TIME], &cs, 2, 0, 1);

    update(el);
}

widget_desc_t __widget_watch_screen = {
    .draw = draw,
    .update = update,
    .ctx_size = sizeof(watch_screen_ctx_t)
};
