#include <stdio.h>
#include <stdint.h>

#include "emu_lcd.h"
#include "emu_graphic.h"

#include "lcd_text_color.h"

#include "dp.h"

#include "text_label.h"
#include "draw_color.h"

#include "timers_32.h"
#include "systick.h"

#include "date_time.h"

enum color_palette {
    COLOR_IDX_BG_UNSEL,
    COLOR_IDX_BG_SEL,
    COLOR_IDX_FG_UNSEL,
    COLOR_IDX_FG_SEL,
    COLOR_IDX_ALT
};

lcd_color_t color_palette[] = {
    [COLOR_IDX_BG_UNSEL] =  COLOR(0x111111),
    [COLOR_IDX_BG_SEL]   =  COLOR(0x113222),

    [COLOR_IDX_FG_UNSEL] =  COLOR(0x555555),
    [COLOR_IDX_FG_SEL] =    COLOR(0x666666),
    [COLOR_IDX_ALT + 0] =   COLOR(0x96A41d),
    [COLOR_IDX_ALT + 1] =   COLOR(0x1fd11E),
    [COLOR_IDX_ALT + 2] =   COLOR(0x12fa44),
    [COLOR_IDX_ALT + 3] =   COLOR(0xE6A7bd),
};

timer_32_t t;

typedef struct {
    val_text_t vt;
    uint32_t time;
    time_t t;
    uint8_t cntr;
} uv_t;

uv_t real_ctx = { .cntr = 12, .time = (3600 * 9) + (60 * 43) + 12, .vt = { .f = X10, .p = 1, .zl = 1, .zr = 1 } }, new_ctx, prev_ctx;

void app_update_ctx(uv_t * ctx, unsigned x)
{
    dp("call app_update_ctx "); dpx((unsigned)ctx, 4); dp(" , x = "); dpd(x, 10); dn();

    dp("cntr = "); dpd(real_ctx.cntr, 10); dn();
    dp("time = "); dpd(real_ctx.time, 10); dn();
    *ctx = real_ctx;

    dp("- ctx new  "); dpx((unsigned)&new_ctx, 4); dp(" : "); dpxd(&new_ctx, 1, sizeof(uv_t)); dn();
    dp("- ctx prev "); dpx((unsigned)&prev_ctx, 4); dp(" : "); dpxd(&prev_ctx, 1, sizeof(uv_t)); dn();
}

#include "stddef.h"

static widget_labels_t widget_labels = {
    .color_palette = color_palette,
    .label = {
        .type = LABEL_SUB_LIST,
        .sublist = &(label_sublist_t){
            .update_func = { .u = (void(*)(void *, unsigned))app_update_ctx },
            .labels_static = LIST(((label_t []){
                { .type = LABEL_STATIC_TEXT, .text = "fuck", .pos = (xy_t){2, 1}, .color_idx = COLOR_IDX_ALT + 0 },
                { .type = LABEL_STATIC_TEXT, .text = "you", .pos = (xy_t){5, 2}, .color_idx = COLOR_IDX_ALT + 2 },
            })),
            .labels_dynamic = LIST(((label_t []){
                { .type = LABEL_VAL, .vt = { .f = X1, .p = 3, .zl = 1, .zr = 1 }, .val_rep.vs = VAL_SIZE_8, .color_idx = COLOR_IDX_ALT + 3, .val_offset_in_ctx = offsetof(uv_t, cntr), .pos = (xy_t){4, 3}, .len = 8 },
                { .type = LABEL_VAL, .val_rep.vs = VAL_SIZE_32, .color_idx = COLOR_IDX_FG_UNSEL, .val_offset_in_ctx = offsetof(uv_t, time), .pos = (xy_t){4, 6}, .len = 10 },
                { .type = LABEL_SUB_LIST, .pos = (xy_t){14, 2}, .val_offset_in_ctx = offsetof(uv_t, time), .val_rep.vs = VAL_SIZE_32, .sub_list_ctx_offset = offsetof(uv_t, t), .sublist = &(label_sublist_t){
                    .update_func = (void(*)(void *, unsigned))time_from_s,
                    .labels_static = LIST(((label_t []){
                        { .type = LABEL_STATIC_TEXT, .text = ":", .pos = (xy_t){2, 0}, .color_idx = COLOR_IDX_FG_UNSEL },
                        { .type = LABEL_STATIC_TEXT, .text = ":", .pos = (xy_t){5, 0}, .color_idx = COLOR_IDX_FG_UNSEL },
                    })),
                    .labels_dynamic = LIST(((label_t []){
                        { .type = LABEL_VAL, .vt = { .f = X1, .p = 0, .zl = 1, .zr = 1 }, .color_idx = COLOR_IDX_ALT + 0, .val_offset_in_ctx = offsetof(time_t, h), .pos.x = 0, . len = 2},
                        { .type = LABEL_VAL, .vt = { .f = X1, .p = 0, .zl = 1, .zr = 1 }, .color_idx = COLOR_IDX_ALT + 2, .val_offset_in_ctx = offsetof(time_t, m), .pos.x = 3, . len = 2},
                        { .type = LABEL_VAL, .vt = { .f = X1, .p = 0, .zl = 1, .zr = 1 }, .color_idx = COLOR_IDX_ALT + 3, .val_offset_in_ctx = offsetof(time_t, s), .pos.x = 6, . len = 2},
                    }))
                } },
            }))
        }
    }
};

tptr_t tp;

void process(char k)
{
    (void)k;
    if (t32_is_over(&t, systick_get_uptime_ms())) {
        t32_extend(&t, 1000);
        real_ctx.time++;
        dn();
        dp("-- updated time "); dpd(real_ctx.time, 10); dn();
        widget_labels_proces(&widget_labels, &tp, &new_ctx, &prev_ctx);
    }
}

int main()
{
    dpn("test text label");

    emu_lcd_cfg_t lcd_cfg = {
        .size = { .w = 320, .h = 240 },
        .scale = 3,
        .px_gap = 0,
        .border = 10,
        .bg_color = 0x202020
    };

    form_t lcd_form;

    emu_lcd_init(&lcd_cfg, &lcd_form);
    emu_graphic_init_xy(lcd_form.s);
    emu_lcd_clear();

    dp("sizeof label_t "); dpd(sizeof(label_t), 3); dn();
    dp("sizeof list_t "); dpd(sizeof(list_t), 3); dn();
    dp("sizeof int "); dpd(sizeof(int), 3); dn();
    dp("sizeof int * "); dpd(sizeof(int *), 3); dn();

    form_t screen_form = { .s = lcd_cfg.size };
    draw_color_form(&screen_form, color_palette[0]);

    extern const font_t font_5x7;

    lcd_font_cfg_t fcfg = {
        .font = &font_5x7,
        .gaps = (xy_t){2, 4},
        .scale = 2
    };

    text_ptr_init(&tp, &fcfg, (xy_t){8, 9}, fcfg_text_char_places(&fcfg, lcd_cfg.size) );
    t32_run(&t, systick_get_uptime_ms(), 1000);

    widget_labels_proces(&widget_labels, &tp, &prev_ctx, 0);

    emu_graphic_loop(process);

    return 0;
}
