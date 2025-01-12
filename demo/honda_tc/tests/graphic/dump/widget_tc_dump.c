#include "widget_tc_dump.h"
#include "draw_color.h"
#include "lcd_text_color.h"
#include "str_val_buf.h"
#include "str_val.h"

#include "dlc_poll.h"

extern font_t font_5x7;

static const lcd_font_cfg_t fcfg = {
    .font = &font_5x7,
    .gaps = { .x = 1, .y = 6 },
};

enum {
    CS_TITLE,
    CS_OFFSETS,
    CS_DATA_0,
    CS_DATA_1,
    CS_DATA_CHANGED
};

color_scheme_t cs[] = {
    [CS_TITLE]          = { .fg = 0xFF0000, .bg = 0 },
    [CS_OFFSETS]        = { .fg = 0xFF00FF, .bg = 0 },
    [CS_DATA_0]         = { .fg = 0xFFFF00, .bg = 0 },
    [CS_DATA_1]         = { .fg = 0x00FF00, .bg = 0 },
    [CS_DATA_CHANGED]   = { .fg = 0xFF0000, .bg = 0 },
};

typedef struct {
    uint8_t data[256];
    xy_t dump_pos;
    xy_t limit_c;
    uint16_t first;
    uint8_t unit;
} ctx_t;

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // dlc_dump_request(HONDA_UNIT_TC);
    uint8_t data[16];
    unsigned address;
    unsigned len = dlc_dump_get_new_data(data, &address);
    if (len) {
        unsigned dump_line = address / 16;
        xy_t dump_bype_pos_c = { .x = 5, .y = 1 + dump_line };
        uint16_t first_mask = (1 << dump_line);
        color_scheme_t * line_cs = &cs[CS_DATA_0 + (dump_line & 1)];

        for (unsigned i = 0; i < 16; i++) {
            if (ctx->first & first_mask) {
                ctx->data[address + i] = data[i];
            }
            char * str = str_val_buf_get();
            hex_to_str(&data[i], str, 1);
            color_scheme_t * byte_cs = line_cs;
            if (ctx->data[address + i] != data[i]) {
                byte_cs = &cs[CS_DATA_CHANGED];
            }
            lcd_color_text_raw_print(str, &fcfg, byte_cs, &ctx->dump_pos, &ctx->limit_c, &dump_bype_pos_c, 2);
            dump_bype_pos_c.x += 3;
        }
        ctx->first &= ~first_mask;

        if (address + 16 >= honda_dlc_unit_len(ctx->unit)) {
            printf("dump request\n");
            dlc_dump_request(ctx->unit);
        }
    }
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->dump_pos = el->f.p;
    ctx->first = -1;
    ctx->unit = HONDA_UNIT_ECU;
    dlc_dump_request(ctx->unit);

    draw_color_form(&el->f, 0);
    ctx->limit_c = lcd_text_char_places(&fcfg, el->f.s);
    // printf("limit_c %d %d\n", limit_c.x, limit_c.y);
    for (unsigned i = 0; i < 16 ; i++) {
        char * str = str_val_buf_get();
        hex_to_str(&i, str, 1);
        lcd_color_text_raw_print(str, &fcfg, &cs[CS_OFFSETS], &ctx->dump_pos, &ctx->limit_c, &(xy_t){ .x = 5 + (3 * i) }, 2);
        unsigned line_offset = i * 16;
        hex_to_str(&line_offset, str, 1);
        lcd_color_text_raw_print(str, &fcfg, &cs[CS_OFFSETS], &ctx->dump_pos, &ctx->limit_c, &(xy_t){ .y = 1 + i }, 2);
        lcd_color_text_raw_print(":", &fcfg, &cs[CS_OFFSETS], &ctx->dump_pos, &ctx->limit_c, &(xy_t){ .x = 2, .y = 1 + i }, 1);
    }
}

const widget_desc_t __widget_tc_dump = {
    .draw = draw,
    .update = update,
    .ctx_size = sizeof(ctx_t)
};
