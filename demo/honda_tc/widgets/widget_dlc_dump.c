#include "widget_dlc_dump.h"
#include "draw_color.h"
#include "lcd_text_color.h"
#include "lcd_color.h"
#include "text_field.h"

#include "str_val_buf.h"
#include "str_val.h"

#include "dlc_poll.h"

#include "event_list.h"
#include "val_mod.h"

#include "dp.h"

extern font_t font_5x7;

// static const lcd_font_cfg_t fcfg = {
//     .font = &font_5x7,
//     .gaps = { .x = 1, .y = 6 },
// };

// text_field_t title_field = {
//     .fcfg = &fcfg,
//     .a = ALIGN_LIC,
//     .padding = { .x = 10, .y = 4 },
// };

tf_cfg_t dump_tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .x = 1, .y = 5 },
        .scale = 1
    },
    // .limit_char = { .x = 3 + (16 * 3), .y = 1 + 8 },
    .a = ALIGN_CC,
    .padding = { .x = 2, .y = 2 }
};

enum {
    CS_TITLE_UNSELECTED,
    CS_TITLE_SELECTED,
    CS_OFFSETS,
    CS_DATA_0,
    CS_DATA_1,
    CS_DATA_CHANGED
};

static const color_scheme_t cs[] = {
    [CS_TITLE_UNSELECTED]   = { .fg = COLOR(0x000000), .bg = COLOR(0xA36A00) },
    [CS_TITLE_SELECTED]     = { .fg = COLOR(0x000000), .bg = COLOR(0xFFB62E) },
    [CS_OFFSETS]            = { .fg = COLOR(0xFF00FF), .bg = COLOR(0) },
    [CS_DATA_0]             = { .fg = COLOR(0xFFFF00), .bg = COLOR(0) },
    [CS_DATA_1]             = { .fg = COLOR(0x00FF00), .bg = COLOR(0) },
    [CS_DATA_CHANGED]       = { .fg = COLOR(0xFF0000), .bg = COLOR(0) },
};

typedef struct {
    uint8_t data[256];
    tf_ctx_t tf;

    form_t title;
    xy_t title_pos;

    form_t dump;
    xy_t dump_pos;
    xy_t limit_c;

    uint16_t first;
    uint8_t unit;
    uint8_t new_unit;
} ctx_t;


static void dump_print_hex(tf_ctx_t * tf, xy_t xy, uint8_t data, const color_scheme_t * cs)
{
    char * str = str_val_buf_get();
    hex_to_str(&data, str, 1);
    lcd_color_tf_print(str, tf, cs, &xy, 2);
}

static void dump_draw_offsets(tf_ctx_t * tf, unsigned len, const color_scheme_t * cs)
{
    xy_t xy = {};
    xy.x = (-3 * 16) + 1;
    for (unsigned i = 0; i < 16; i++) {
        dump_print_hex(tf, xy, i, cs);
        xy.x += 3;
    }
    xy.x = 0;
    xy.y = -16;

    for (unsigned v = 0; v < len; v += 16) {
        xy_t xy_sep;
        xy_sep.x = 2;
        xy_sep.y = xy.y;

        dump_print_hex(tf, xy, v, cs);
        lcd_color_text_raw_print(":", tf->tfcfg->fcfg, cs, &tf->xy, &tf->size, &xy_sep, 1);
        xy.y++;
    }
}

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    uint8_t data[16];
    unsigned address;
    honda_unit_t unit;
    unsigned len = dlc_dump_get_new_data(data, &address, &unit);

    if (unit != ctx->unit) {
        return;
    }

    if (len) {
        // dpn("dump data ready");
        unsigned dump_line = address / 16;
        uint16_t first_mask = (1 << dump_line);
        xy_t byte_xy = { .x = (-3 * 16) + 1, .y = -16 + dump_line };
        const color_scheme_t * line_cs = &cs[CS_DATA_0 + (dump_line & 1)];

        for (unsigned i = 0; i < len; i++) {
            // копирование данных в контекст для понимания изменений в дальнейшем
            if (ctx->first & first_mask) {
                ctx->data[address + i] = data[i];
            }

            const color_scheme_t * byte_cs = line_cs;
            if (ctx->data[address + i] != data[i]) {
                byte_cs = &cs[CS_DATA_CHANGED];
            }
            dump_print_hex(&ctx->tf, byte_xy, data[i], byte_cs);
            byte_xy.x += 3;
        }
        ctx->first &= ~first_mask;

        if (address + len >= honda_dlc_unit_len(ctx->unit)) {
            // printf("dump request\n");
            dlc_dump_request(ctx->unit);
        }
    }
}


// static void redraw_unit_name(ctx_t * ctx, unsigned active)
// {
//     color_scheme_t * c = &cs[CS_TITLE_UNSELECTED + active];
//     lcd_color_text_raw_print(honda_dlc_unit_name(ctx->new_unit), &fcfg, c, &ctx->title_pos, 0, &(xy_t){ .x = TEXT_LEN("DLC DUMP unit") + 1}, 0);
//     if (ctx->new_unit != ctx->unit) {
//         lcd_color_text_raw_print("(press OK to select UNIT)", &fcfg, c, &ctx->title_pos, 0, &(xy_t){ .x = TEXT_LEN("DLC DUMP unit") + 1 + 3 + 1 }, 0);
//     } else {
//         lcd_color_text_raw_print(0, &fcfg, c, &ctx->title_pos, 0, &(xy_t){ .x = TEXT_LEN("DLC DUMP unit") + 1 + 3 + 1 }, TEXT_LEN("(press OK to select UNIT)"));
//     }
// }

// static void select(ui_element_t * el)
// {
//     ctx_t * ctx = (ctx_t *)el->ctx;
//     color_scheme_t * c = &cs[CS_TITLE_UNSELECTED + el->active];
//     draw_color_form(&ctx->title, c->bg);
//     lcd_color_text_raw_print("DLC DUMP unit", &fcfg, c, &ctx->title_pos, 0, 0, 0);
//     redraw_unit_name(ctx, el->active);
// }

// static void draw(ui_element_t * el)
// {
//     ctx_t * ctx = (ctx_t *)el->ctx;
//     select(el);
//     redraw_dump(ctx);
// }

// unsigned process(ui_element_t * el, unsigned event)
// {
//     ctx_t * ctx = (ctx_t *)el->ctx;
//     if (event == EVENT_BTN_OK) {
//         if (ctx->unit != ctx->new_unit) {
//             ctx->unit = ctx->new_unit;
//             redraw_unit_name(ctx, el->active);
//             redraw_dump(ctx);
//             // printf("dump new dlc unit %d\n", ctx->unit);
//             return 1;
//         }
//     } else if (event == EVENT_BTN_UP) {
//         if (val_mod_unsigned(&ctx->new_unit, VAL_SIZE_8, MOD_OP_ADD, 1, 0, HONDA_UNIT_COUNT - 1, 1)) {
//             redraw_unit_name(ctx, el->active);
//         }
//         return 1;
//     } else if (event == EVENT_BTN_DOWN) {
//         if (val_mod_unsigned(&ctx->new_unit, VAL_SIZE_8, MOD_OP_SUB, 1, 0, HONDA_UNIT_COUNT - 1, 1)) {
//             redraw_unit_name(ctx, el->active);
//         }
//         return 1;
//     } else if (event == EVENT_BTN_BACK) {
//         if (ctx->new_unit != ctx->unit) {
//             ctx->new_unit = ctx->unit;
//             redraw_unit_name(ctx, el->active);
//             return 1;
//         }
//     }
//     return 0;
// }

static void draw(ui_element_t * el)
{
    // printf("dump draw\n");
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->first = -1;
    ctx->unit = el->idx;

    tf_ctx_calc(&ctx->tf, &el->f, &dump_tf);
    draw_color_form(&el->f, cs[CS_OFFSETS].bg);
    dump_draw_offsets(&ctx->tf, honda_dlc_unit_len(ctx->unit), &cs[CS_OFFSETS]);

    // тут делаем press ok to request dump
    dlc_dump_request(ctx->unit);

    el->drawed = 1;
}

const widget_desc_t widget_dlc_dump = {
    .draw = draw,
    .update = update,
    // .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
