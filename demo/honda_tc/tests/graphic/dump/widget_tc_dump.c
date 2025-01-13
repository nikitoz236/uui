#include "widget_tc_dump.h"
#include "draw_color.h"
#include "lcd_text_color.h"
#include "str_val_buf.h"
#include "str_val.h"

#include "dlc_poll.h"
#include "text_field.h"
#include "forms_align.h"
#include "forms_split.h"
#include "event_list.h"
#include "val_mod.h"

extern font_t font_5x7;

static const lcd_font_cfg_t fcfg = {
    .font = &font_5x7,
    .gaps = { .x = 1, .y = 6 },
};

text_field_t title_field = {
    .fcfg = &fcfg,
    .a = ALIGN_LIC,
    .padding = { .x = 10, .y = 4 },
};

enum {
    CS_TITLE_UNSELECTED,
    CS_TITLE_SELECTED,
    CS_OFFSETS,
    CS_DATA_0,
    CS_DATA_1,
    CS_DATA_CHANGED
};

color_scheme_t cs[] = {
    [CS_TITLE_UNSELECTED]   = { .fg = 0x000000, .bg = 0xA36A00 },
    [CS_TITLE_SELECTED]     = { .fg = 0x000000, .bg = 0xFFB62E },
    [CS_OFFSETS]            = { .fg = 0xFF00FF, .bg = 0 },
    [CS_DATA_0]             = { .fg = 0xFFFF00, .bg = 0 },
    [CS_DATA_1]             = { .fg = 0x00FF00, .bg = 0 },
    [CS_DATA_CHANGED]       = { .fg = 0xFF0000, .bg = 0 },
};

typedef struct {
    uint8_t data[256];
    form_t title;
    form_t dump;
    xy_t title_pos;
    xy_t dump_pos;
    xy_t limit_c;
    uint16_t first;
    uint8_t unit;
    uint8_t new_unit;
} ctx_t;

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    // dlc_dump_request(HONDA_UNIT_TC);
    uint8_t data[16];
    unsigned address;
    honda_unit_t unit;
    unsigned len = dlc_dump_get_new_data(data, &address, &unit);

    /*
        здесь нам нужно переделать возвращаемые данные функцией dlc_dump_get_new_data
    */

    if (unit != ctx->unit) {
        return;
    }
    if (len) {
        unsigned dump_line = address / 16;
        xy_t dump_bype_pos_c = { .x = 4, .y = 1 + dump_line };
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

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->unit = HONDA_UNIT_ECU;
    ctx->new_unit = ctx->unit;

    ctx->title = el->f;
    ctx->title.s.h = text_field_size_px(&title_field).h;
    ctx->title_pos = text_field_text_pos(&ctx->title, &title_field);

    ctx->dump = el->f;
    printf("dump form: p[%d %d]s(%d %d)\n", ctx->dump.p.x, ctx->dump.p.y, ctx->dump.s.x, ctx->dump.s.y);
    form_cut(&ctx->dump, DIMENSION_Y, EDGE_U, ctx->title.s.h);
    xy_t padding = { .x = 6, .y = 4 };
    ctx->dump_pos = align_form_pos(&ctx->dump, (xy_t){}, ALIGN_LIUI, padding);

    ctx->limit_c = lcd_text_char_places(&fcfg, size_sub_padding(ctx->dump.s, padding));
}

static void redraw_unit_name(ctx_t * ctx, unsigned active)
{
    color_scheme_t * c = &cs[CS_TITLE_UNSELECTED + active];
    lcd_color_text_raw_print(honda_dlc_unit_name(ctx->new_unit), &fcfg, c, &ctx->title_pos, 0, &(xy_t){ .x = TEXT_LEN("DLC DUMP unit") + 1}, 0);
    if (ctx->new_unit != ctx->unit) {
        lcd_color_text_raw_print("(press OK to select UNIT)", &fcfg, c, &ctx->title_pos, 0, &(xy_t){ .x = TEXT_LEN("DLC DUMP unit") + 1 + 3 + 1 }, 0);
    } else {
        lcd_color_text_raw_print(0, &fcfg, c, &ctx->title_pos, 0, &(xy_t){ .x = TEXT_LEN("DLC DUMP unit") + 1 + 3 + 1 }, TEXT_LEN("(press OK to select UNIT)"));
    }
}

static void select(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    color_scheme_t * c = &cs[CS_TITLE_UNSELECTED + el->active];
    draw_color_form(&ctx->title, c->bg);
    lcd_color_text_raw_print("DLC DUMP unit", &fcfg, c, &ctx->title_pos, 0, 0, 0);
    redraw_unit_name(ctx, el->active);
}

static void redraw_dump(ctx_t * ctx)
{
    dlc_dump_request(ctx->unit);

    ctx->first = -1;
    draw_color_form(&ctx->dump, cs[CS_OFFSETS].bg);

    for (unsigned i = 0; i < 16; i++) {
        char * str = str_val_buf_get();
        hex_to_str(&i, str, 1);
        lcd_color_text_raw_print(str, &fcfg, &cs[CS_OFFSETS], &ctx->dump_pos, &ctx->limit_c, &(xy_t){ .x = 4 + (3 * i) }, 2);
    }

    unsigned lines = honda_dlc_unit_len(ctx->unit) / 16;
    for (unsigned i = 0; i < lines; i++) {
        unsigned line_offset = i * 16;
        char * str = str_val_buf_get();
        hex_to_str(&line_offset, str, 1);
        lcd_color_text_raw_print(str, &fcfg, &cs[CS_OFFSETS], &ctx->dump_pos, &ctx->limit_c, &(xy_t){ .y = 1 + i }, 2);
        lcd_color_text_raw_print(":", &fcfg, &cs[CS_OFFSETS], &ctx->dump_pos, &ctx->limit_c, &(xy_t){ .x = 2, .y = 1 + i }, 1);
    }
}

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    select(el);
    redraw_dump(ctx);
}

unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    if (event == EVENT_BTN_OK) {
        if (ctx->unit != ctx->new_unit) {
            ctx->unit = ctx->new_unit;
            redraw_unit_name(ctx, el->active);
            redraw_dump(ctx);
            printf("dump new dlc unit %d\n", ctx->unit);
            return 1;
        }
    } else if (event == EVENT_BTN_UP) {
        if (val_mod_unsigned(&ctx->new_unit, VAL_SIZE_8, MOD_OP_ADD, 1, 0, HONDA_UNIT_COUNT - 1, 1)) {
            redraw_unit_name(ctx, el->active);
        }
        return 1;
    } else if (event == EVENT_BTN_DOWN) {
        if (val_mod_unsigned(&ctx->new_unit, VAL_SIZE_8, MOD_OP_SUB, 1, 0, HONDA_UNIT_COUNT - 1, 1)) {
            redraw_unit_name(ctx, el->active);
        }
        return 1;
    } else if (event == EVENT_BTN_BACK) {
        if (ctx->new_unit != ctx->unit) {
            ctx->new_unit = ctx->unit;
            redraw_unit_name(ctx, el->active);
            return 1;
        }
    }
    return 0;
}

const widget_desc_t __widget_tc_dump = {
    .draw = draw,
    .update = update,
    .select = select,
    .extend = extend,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
