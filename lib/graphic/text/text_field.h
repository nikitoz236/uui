#pragma once

#include "lcd_text.h"
#include "val_text.h"
#include "forms_align.h"

typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t limit_char;
    xy_t margin;
    xy_t padding;       // ???
    align_t a;
} text_field_t;

typedef struct {
    const text_field_t * tfcfg;                     // текстовое поле
    union {
        const char * text;
        const char * (*to_str)(unsigned idx);
    };
    xy_t pos;                                       // положение знакоместа
    uint16_t len;                                   // в какую длину поля поместить
    val_text_t vt;                                  // отрисовка
    val_rep_t rep;                                  // представление в памяти
    uint16_t offset;                                // смещение в памяти
} text_field_label_t;

typedef struct {
    const text_field_t * tfcfg;
    const char * (*to_str)(unsigned x);
    const val_text_t * vt;
    xy_t pos;
    uint16_t len;
} text_field_value_t;

static inline xy_t text_field_size_px(const text_field_t * cfg)
{
    return size_add_padding(lcd_text_size_px(cfg->fcfg, cfg->limit_char), cfg->padding);
}

static inline xy_t text_field_text_pos(const form_t * f, const text_field_t * cfg)
{
    return align_form_pos(f, lcd_text_size_px(cfg->fcfg, cfg->limit_char), cfg->a, cfg->padding);
}


/*

итак значит мне сейчас надо сейчас нахуевертить механизм отрисовки чегонибудь

у тебя есть текстовое поле и к нему массив структур, количество структур.

даже несколько массивов. статический и динамический

для статической надписи тебе надо либо указатель на текст, либо на указатель по индексу

для динамической надписи тебе надо указатель на функцию которая преобразует значение в текст

*/

typedef struct {
    const char * text;
    const char * (*to_str)(unsigned x);
    xy_t xy;
    uint16_t len;
} label_static_t;

struct sub_label_list;

typedef struct {
    union {
        const char * (*to_str)(unsigned x);
        const struct sub_label_list * sl;
        val_text_t vt;
    };
    xy_t xy;
    union {
        uint16_t len;
        uint16_t sub_ctx_offset;
    };
    uint16_t offset;
    val_rep_t rep;
    enum {
        LV,
        LF,
        LS
    } t : 2;
} label_value_t;

typedef struct {
    label_value_t * label_list;
    uint16_t ctx_offset;
    uint16_t ctx_size;
} sub_label_list_t;

typedef struct {
    const text_field_t * tfcfg;
    xy_t pos;
} tf_ctx_t;

struct sub_label_list {
    void (*ctx_update)(void * ctx, unsigned x);
    const label_value_t * list;
    uint8_t count;
};


// typedef struct {
//     union {
//         const char * text;
//         const char * (*to_str)(unsigned x);
//         val_text_t vt;
//     };
//     xy_t pos_char;
// };


