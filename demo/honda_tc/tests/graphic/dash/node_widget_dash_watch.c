#include "ui_tree.h"
#include "draw_color.h"
#include "text_label_color.h"
#include "date_time.h"
#include "rtc.h"
#include "time_zone.h"
#include "lcd_text_color.h"

#define COLOR_FG        COLOR(0xE76a35)
#define COLOR_BG        COLOR(0x3A0D12)

extern const font_t font_5x7;

static const tf_cfg_t tf_dash_watch = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .scale = 5,
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
    .wrap_list = (label_color_t []) { { .color = COLOR_FG, .l = { .t = LP_S, .ofs = offsetof(uv_t, s), .rep = { .vs = VAL_SIZE_32 }, .sofs = offsetof(uv_t, t), .sl = &(const label_list_t){ 
        .ctx_update = (void(*)(void *, unsigned))time_from_s, .count = 2, .list = (label_t []) {
            { .len = 2, .t = LP_V, .xy = { .x = 0 }, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .ofs = offsetof(time_t, h) },
            { .len = 2, .t = LP_V, .xy = { .x = 3 }, .vt = { .zl = 1 }, .rep = { .vs = VAL_SIZE_8 }, .ofs = offsetof(time_t, m) },
        } } } },
    }
};

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    label_color_list(&ctx->tf, &ll_var, COLOR_BG, 0, &ctx->uv, 0);
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    draw_color_form(&el->f, COLOR_BG);
    tf_ctx_calc(&ctx->tf, &el->f, &tf_dash_watch);
    printf("watch tf size %d %d\n", ctx->tf.size.x, ctx->tf.size.y);
    lcd_color_text_raw_print(":", tf_dash_watch.fcfg, &(color_scheme_t) { .bg = COLOR_BG, .fg = COLOR_FG }, &ctx->tf.xy, &ctx->tf.size, &(xy_t){ .x = 2 }, 1);
    label_color_list(&ctx->tf, &ll_var, COLOR_BG, 0, &ctx->uv, 0);
    el->drawed = 1;
}

const ui_node_desc_t node_widget_dash_watch = {
    .widget = &(const widget_desc_t){
        .draw = draw,
        .update = update,
        .ctx_size = sizeof(ctx_t),
    }
};
