#include <stdio.h>
#include <stdint.h>

#include "emu_lcd.h"
#include "emu_graphic.h"

#include "lcd_text_color.h"

#define DP_NOTABLE
#include "dp.h"

#include "text_label.h"
#include "draw_color.h"



#include "date_time.h"

typedef struct {
    val_text_t vt;
    uint32_t time;
    time_t t;
    uint8_t cntr;
} uv_t;

uv_t real_ctx = { .cntr = 12, .time = 873654, .vt = { .f = X10, .p = 1, .zl = 1, .zr = 1 } }, new_ctx, prev_ctx;

void app_update_ctx(uv_t * ctx, unsigned x)
{
    dp("call app_update_ctx, x = "); dpd(x, 10); dn();
    real_ctx.cntr += x;
    real_ctx.time += 19;

    dp("cntr = "); dpd(real_ctx.cntr, 10); dn();
    dp("time = "); dpd(real_ctx.time, 10); dn();
    *ctx = real_ctx;
}


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
    [COLOR_IDX_ALT + 1] =   COLOR(0x12fa44),
    [COLOR_IDX_ALT + 2] =   COLOR(0x12fa44),
    [COLOR_IDX_ALT + 3] =   COLOR(0xE6A7bd),
};

#include "stddef.h"

static widget_labels_t widget_labels = {
    .color_palette = color_palette,
    .static_labels = LIST(((label_t []){
        { .type = LABEL_STATIC_TEXT, .text = "fuck", .pos = (xy_t){2, 1}, .color_idx = COLOR_IDX_ALT + 1 },
        { .type = LABEL_STATIC_TEXT, .text = "you", .pos = (xy_t){5, 2}, .color_idx = COLOR_IDX_ALT + 2 },
    })),
    .label = {
        .type = LABEL_SUB_LIST,
        .sublist = {
            .update_func = { .u = app_update_ctx },
            .labels = LIST(((label_t []){
                { .type = LABEL_VAL, .vt_real = 1, .vt = { .f = X1, .p = 3, .zl = 1, .zr = 1 }, .val_rep.vs = VAL_SIZE_8, .color_idx = COLOR_IDX_ALT + 3, .val_offset_in_ctx = offsetof(uv_t, cntr), .pos = (xy_t){4, 3}, .len = 8 },
                         // { .type = LABEL_VAL, .vt_real = 1, .vt = { .f = X10, .p = 1, .zl = 1, .zr = 1 }, .color_idx = COLOR_IDX_FG_UNSEL, .idx_or_val = 0, .pos = (xy_t){4, 14} },
                { .type = LABEL_SUB_LIST, .pos = (xy_t){14, 2}, .val_offset_in_ctx = offsetof(uv_t, time), .sub_list_ctx_offset = offsetof(uv_t, t), .sublist = { .update_func = time_from_s,
                    .labels = LIST(((label_t []){
                        { .type = LABEL_VAL, .vt_real = 1, .vt = { .f = X1, .p = 0, .zl = 1, .zr = 1 }, .color_idx = COLOR_IDX_ALT + 1, .val_offset_in_ctx = offsetof(time_t, h), .pos.x = 0, . len = 2},
                        { .type = LABEL_VAL, .vt_real = 1, .vt = { .f = X1, .p = 0, .zl = 1, .zr = 1 }, .color_idx = COLOR_IDX_ALT + 2, .val_offset_in_ctx = offsetof(time_t, m), .pos.x = 3, . len = 2},
                        { .type = LABEL_VAL, .vt_real = 1, .vt = { .f = X1, .p = 0, .zl = 1, .zr = 1 }, .color_idx = COLOR_IDX_ALT + 3, .val_offset_in_ctx = offsetof(time_t, s), .pos.x = 6, . len = 2},
                    }))
                } },
            }))
        }
    }
};


tptr_t tp;

void process(unsigned k)
{
    widget_labels_proces(&widget_labels, &tp, &new_ctx, &prev_ctx);
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
    // dp("sizeof struct list_test_38 "); dpd(sizeof(), 3); dn();

    // struct list_test * s = (struct list_test *)widget_labels.static_labels;

    // dpx((unsigned)s, 4); dp(" ");
    // dpx((unsigned)&s->items[0], 4);
    // dn();


    form_t screen_form = { .s = lcd_cfg.size };
    draw_color_form(&screen_form, color_palette[0]);

    extern const font_t font_5x7;

    lcd_font_cfg_t fcfg = {
        .font = &font_5x7,
        .gaps = (xy_t){2, 4},
        .scale = 2
    };

    text_ptr_init(&tp, &fcfg, (xy_t){8, 9}, fcfg_text_char_places(&fcfg, (xy_t){300, 200}) );


    widget_labels_proces(&widget_labels, &tp, &new_ctx, 0);


    emu_graphic_loop(process);

    return 0;
}
