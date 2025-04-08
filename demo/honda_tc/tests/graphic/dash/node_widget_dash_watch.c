#include "ui_tree.h"
#include "draw_color.h"
#include "text_label_color.h"
#include "date_time.h"
#include "rtc.h"
#include "time_zone.h"
#include "lcd_text_color.h"

extern const font_t font_3x5;

static const tf_cfg_t tf_dash_watch = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_3x5,
        .scale = 8,
        .gaps = { .x = 5 },
    },
    .padding = { .x = 0, .y = 0 },
    .a = ALIGN_CC
};

typedef struct {
    unsigned s;
    time_t t;
} uv_t;

typedef struct {
    tf_ctx_t tf;
    color_scheme_t cs;
    uv_t uv;
} ctx_t;

static void ctx_update_watch(uv_t * uv, unsigned idx)
{
    (void)idx;
    uv->s = rtc_get_time_s() + time_zone_get();
}

static const label_list_t ll_var = {
    .count = 1,
    .ctx_update = (void(*)(void *, unsigned))ctx_update_watch,
    .list = (label_t []) { { .t = LP_S, .ofs = offsetof(uv_t, s), .rep = { .vs = VAL_SIZE_32 }, .sofs = offsetof(uv_t, t), .sl = &(const label_list_t) {
        .ctx_update = (void(*)(void *, unsigned))time_from_s, .count = 2, .list = (label_t []) {
            { .len = 2, .t = LP_V, .xy = { .x = 0 }, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .ofs = offsetof(time_t, h) },
            { .len = 2, .t = LP_V, .xy = { .x = 3 }, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .ofs = offsetof(time_t, m) },
        } } },
    }
};

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    lcd_label_list(&ctx->tf, &ll_var, &ctx->cs, 0, &ctx->uv, 0);
}

color_scheme_t dash_get_cs(unsigned idx);

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->cs = dash_get_cs(el->idx);
    draw_color_form(&el->f, ctx->cs.bg);
    tf_ctx_calc(&ctx->tf, &el->f, &tf_dash_watch);
    // printf("watch tf size %d %d\n", ctx->tf.size.x, ctx->tf.size.y);
    lcd_color_text_raw_print(":", tf_dash_watch.fcfg, &ctx->cs, &ctx->tf.xy, &ctx->tf.size, &(xy_t){ .x = 2 }, 1);
    lcd_label_list(&ctx->tf, &ll_var, &ctx->cs, 0, &ctx->uv, 0);
    el->drawed = 1;
}

const ui_node_desc_t node_widget_dash_watch = {
    .widget = &(const widget_desc_t){
        .draw = draw,
        .update = update,
        .ctx_size = sizeof(ctx_t),
    }
};
