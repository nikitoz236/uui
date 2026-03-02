#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "emu_graphic.h"
#include "dp.h"

#include "lora_ping_pong.h"

#include "text_label.h"
#include "draw_color.h"
#include "timers_32.h"
#include "uptime.h"

/* ── палитра ───────────────────────────────────────────────────────── */

enum {
    C_BG,
    C_HEADER,
    C_LABEL,
    C_VAL,
    C_RSSI,
};

static lcd_color_t palette[] = {
    [C_BG]     = COLOR(0x101010),
    [C_HEADER] = COLOR(0x607060),
    [C_LABEL]  = COLOR(0x3a6e3a),
    [C_VAL]    = COLOR(0x22cc22),
    [C_RSSI]   = COLOR(0xccaa22),
};

/* ── данные ────────────────────────────────────────────────────────── */

static ping_stats_t real_st, new_st, prev_st;

static void update_ctx(ping_stats_t * ctx, unsigned unused)
{
    (void)unused;
    *ctx = real_st;
}

/* ── макросы лэйблов ───────────────────────────────────────────────── */

#define DV32(field, px, py, l) \
    { .type = LABEL_VAL, .vt = { .f = X1, .zl = 1 }, \
      .val_rep.vs = VAL_SIZE_32, .color_idx = C_VAL, \
      .val_offset_in_ctx = offsetof(ping_stats_t, field), \
      .pos = {(px), (py)}, .len = (l) }

#define DV_S8(field, px, py, l) \
    { .type = LABEL_VAL, .vt = { .f = X1 }, \
      .val_rep = { .vs = VAL_SIZE_8, .s = 1 }, .color_idx = C_RSSI, \
      .val_offset_in_ctx = offsetof(ping_stats_t, field), \
      .pos = {(px), (py)}, .len = (l) }

/* ── виджет ────────────────────────────────────────────────────────── */

/*
 *  480×222, шрифт 5×7, scale=2, gap=4 → 34 кол × 11 строк
 *
 *  col: 0         1         2         3
 *       0123456789012345678901234567890123
 *  y=0  ping seq:  00001
 *
 *  y=2  --- remote ---
 *  y=3  rx cnt:  00001   lost:  00000
 *  y=4    rssi:    -52     snr:     11
 *  y=5  reply seq:  00001
 *
 *  y=7  --- received ---
 *  y=8  rx cnt:  00001   lost:  00000
 *  y=9    rssi:    -48     snr:      8
 */

static widget_labels_t wl = {
    .color_palette = palette,
    .label = {
        .type = LABEL_SUB_LIST,
        .sublist = &(label_sublist_t){
            .update_func = { .u = (void(*)(void *, unsigned))update_ctx },

            .labels_static = LIST(((label_t[]){
                { .type = LABEL_STATIC_TEXT, .text = "ping seq:",        .pos = { 0, 0}, .color_idx = C_LABEL  },

                { .type = LABEL_STATIC_TEXT, .text = "--- remote ---",   .pos = { 0, 2}, .color_idx = C_HEADER },
                { .type = LABEL_STATIC_TEXT, .text = "rx cnt:",          .pos = { 0, 3}, .color_idx = C_LABEL  },
                { .type = LABEL_STATIC_TEXT, .text = "lost:",            .pos = {16, 3}, .color_idx = C_LABEL  },
                { .type = LABEL_STATIC_TEXT, .text = "rssi:",            .pos = { 2, 4}, .color_idx = C_LABEL  },
                { .type = LABEL_STATIC_TEXT, .text = "snr:",             .pos = {18, 4}, .color_idx = C_LABEL  },
                { .type = LABEL_STATIC_TEXT, .text = "reply seq:",       .pos = { 0, 5}, .color_idx = C_LABEL  },

                { .type = LABEL_STATIC_TEXT, .text = "--- received ---", .pos = { 0, 7}, .color_idx = C_HEADER },
                { .type = LABEL_STATIC_TEXT, .text = "rx cnt:",          .pos = { 0, 8}, .color_idx = C_LABEL  },
                { .type = LABEL_STATIC_TEXT, .text = "lost:",            .pos = {16, 8}, .color_idx = C_LABEL  },
                { .type = LABEL_STATIC_TEXT, .text = "rssi:",            .pos = { 2, 9}, .color_idx = C_LABEL  },
                { .type = LABEL_STATIC_TEXT, .text = "snr:",             .pos = {18, 9}, .color_idx = C_LABEL  },
            })),

            .labels_dynamic = LIST(((label_t[]){
                DV32(stat.seq,       10, 0, 5),

                DV32(pong.rx_cnt,     8, 3, 5),
                DV32(pong.rx_lost,   22, 3, 5),
                DV_S8(rem.rssi,       8, 4, 4),
                DV_S8(rem.snr,       23, 4, 4),
                DV32(pong.rx_seq,    11, 5, 5),

                DV32(stat.cnt,        8, 8, 5),
                DV32(stat.lost,      22, 8, 5),
                DV_S8(loc.rssi,       8, 9, 4),
                DV_S8(loc.snr,       23, 9, 4),
            }))
        }
    }
};

/* ── симуляция ─────────────────────────────────────────────────────── */

static void tick(void)
{
    real_st.stat.seq++;

    if (real_st.stat.seq % 7 == 0) {
        real_st.stat.lost++;
    } else {
        real_st.stat.cnt++;

        real_st.rem.rssi = -52 - (int8_t)(real_st.stat.seq % 5);
        real_st.rem.snr  =  11 - (int8_t)(real_st.stat.seq % 3);
        real_st.loc.rssi = -48 - (int8_t)(real_st.stat.seq % 4);
        real_st.loc.snr  =   8 + (int8_t)(real_st.stat.seq % 2);

        real_st.pong.rx_seq  = real_st.stat.seq;
        real_st.pong.rx_lost = (real_st.stat.seq % 7 == 1) ? 1 : 0;
        real_st.pong.rx_cnt  = real_st.stat.cnt;
        real_st.pong.tx_cnt  = real_st.stat.cnt;
        real_st.pong.rssi    = (uint8_t)(-real_st.rem.rssi * 2);
        real_st.pong.snr     = (int8_t)(real_st.rem.snr * 4);
    }
}

/* ── main ──────────────────────────────────────────────────────────── */

static tptr_t tp;
static timer_32_t timer;

static void process(char k)
{
    (void)k;
    if (t32_is_over(&timer, get_uptime_ms())) {
        t32_extend(&timer, 1000);
        tick();
        widget_labels_proces(&wl, &tp, &new_st, &prev_st);
    }
}

int main(void)
{
    emu_lcd_cfg_t lcd_cfg = {
        .size   = { .w = 480, .h = 222 },
        .scale  = 2,
        .px_gap = 0,
        .border = 4,
        .bg_color = 0x101010,
    };

    emu_graphic_init(&lcd_cfg);

    extern const font_t font_5x7;

    lcd_font_cfg_t fcfg = {
        .font  = &font_5x7,
        .gaps  = (xy_t){ 4, 4 },
        .scale = 2,
    };

    form_t screen = { .s = lcd_cfg.size };
    draw_color_form(&screen, palette[C_BG]);

    tp = text_ptr_create((tf_t){
        .fcfg = &fcfg,
        .pos  = (xy_t){ 4, 4 },
        .lim  = fcfg_text_char_places(&fcfg, lcd_cfg.size),
    });

    t32_run(&timer, get_uptime_ms(), 1000);

    widget_labels_proces(&wl, &tp, &prev_st, 0);

    emu_graphic_loop(process);

    return 0;
}
