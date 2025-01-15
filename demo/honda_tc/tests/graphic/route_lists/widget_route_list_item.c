#include "widget_route_list_item.h"
#include "lcd_text_color.h"
#include "text_field.h"
#include "draw_color.h"
#include "array_size.h"

#include "routes.h"


/*
    достаточно типичная ситуация - горизонтальный элемент
    как выглядит работа с ним

    на стадии сфдс вычисляется размер формы - дальше форму такто надо сохранить если это например заголовок чтобы можно было ее закрасить на стадии draw

*/


const lcd_color_t bg[] = { 0x111111, 0x113222 };

typedef struct {
    xy_t tp;
} ctx_t;

extern const font_t font_5x7;

const text_field_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .y = 4 },
    },
    .a = ALIGN_CC,
    .padding = { .x = 8, .y = 8 },
    .limit_char = { .x = 50, .y = 4 },      // ну вот он же может расширятся, и интерфейс может расширятся, есть поля привязаные к правому краю, хм
};

/*
    можно coord_t сделать знаковым, тогда можно вектора сдвиги делать итд, выравнивать по правому краю итд
*/

enum {
    ITEM_DIST
};

const struct {
    text_field_label_t tl;
    lcd_color_t color;
    const val_text_t vt;
    uint8_t is_val;
    uint8_t offset;
} labels[] = {
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 31, .y = 0 }, .text = "time:",         } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 44, .y = 0 }, .text = ":",             } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 47, .y = 0 }, .text = ":",             } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 1 }, .text = "start:",        } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 21, .y = 1 }, .text = ":",             } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 24, .y = 1 }, .text = ":",             } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 32, .y = 1 }, .text = "odo:",          } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 48, .y = 1 }, .text = "km",            } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 2 }, .text = "dist:",         } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 19, .y = 2 }, .text = "km",            } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 31, .y = 2 }, .text = "fuel:",         } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 49, .y = 2 }, .text = "L",             } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 3 }, .text = "avg speed: ",   } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 18, .y = 3 }, .text = "km/h",          } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 31, .y = 3 }, .text = "cons: ",        } },
    { .color = 0x555555, .tl = { .tfcfg = &tf, .pos = { .x = 43, .y = 3 }, .text = "L/100km",       } },

    { .color = 0x882222, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 0 }, .to_str = route_name }, .is_val = 1 },
    // { .color = 0x882222, .tl = { .tfcfg = &tf, .pos = { .x = 0,  .y = 0 }, .text = "JOURNEY",       } },
    { .color = 0xFF0000, .tl = { .tfcfg = &tf, .pos = { .x = 14, .y = 0 }, .text = "OK to RESTART", } },

    { .color = 0xE6A7bd, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 0 }, .text = "1243562",       } },
    { .color = 0xE6A7bd, .tl = { .tfcfg = &tf, .pos = { .x = 45, .y = 0 }, .text = "34",            } },
    { .color = 0xE6A7bd, .tl = { .tfcfg = &tf, .pos = { .x = 48, .y = 0 }, .text = "23",            } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 7,  .y = 1 }, .text = "21",            } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 10, .y = 1 }, .text = "DEC",           } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 14, .y = 1 }, .text = "2024",          } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 19, .y = 1 }, .text = "13",            } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 22, .y = 1 }, .text = "34",            } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 25, .y = 1 }, .text = "23",            } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 1 }, .text = "345674.323",    } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 6,  .y = 2 }, .text = "12345673.343",  } },
    { .color = 0x96A4Ad, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 2 }, .text = "2334567.233",   } },
    { .color = 0x96A41d, .tl = { .tfcfg = &tf, .pos = { .x = 11, .y = 3 }, .text = "253.45",        } },
    { .color = 0x12fa44, .tl = { .tfcfg = &tf, .pos = { .x = 37, .y = 3 }, .text = "37.56",         } },
};

/*
vscode показывает символы начиная с 1, графика с 0

JOURNEY     RESTART? (OK)       time: 124562:34:23
start: 21 DEC 2024 13:34:23     odo: 345674.324 km
dist: 12345673.343 km          fuel: 2334567.233 L
avg speed: 253.45 km/h      cons: 37.56 L / 100 km
*/

static void print_labels(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    for (unsigned i = 0; i < ARRAY_SIZE(labels); i++) {
        char * str = labels[i].tl.text;
        if (labels[i].is_val) {
            str = labels[i].tl.to_str(el->idx);
        }
        color_scheme_t cs = { .fg = labels[i].color, .bg = bg[el->active] };
        lcd_color_text_raw_print(str, labels[i].tl.tfcfg->fcfg, &cs, &ctx->tp, &labels[i].tl.tfcfg->limit_char, &labels[i].tl.pos, 0);
    }
}

static void calc(ui_element_t * el)
{
    el->f.s = text_field_size_px(&tf);
}

static void extend(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->tp = text_field_text_pos(&el->f, &tf);
}

static void draw(ui_element_t * el)
{
    draw_color_form(&el->f, bg[el->active]);
    print_labels(el);
}

const widget_desc_t widget_route_list_item = {
    .calc = calc,
    .extend = extend,
    .draw = draw,
    .select = draw,
    .ctx_size = sizeof(ctx_t),
};