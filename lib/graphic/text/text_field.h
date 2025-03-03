#pragma once

#include "lcd_text.h"
#include "val_text.h"
#include "forms_align.h"
#include <stddef.h>

// получается что это один из вариантов универсального блока интерфейса, допустим еще есть картинки или какаято графика. хз, главное что мы располагая их оперируем формами
typedef struct {
    const lcd_font_cfg_t * fcfg;
    xy_t limit_char;
    // xy_t margin;                // не должно быть. это внешнее свойство как универсальные блоки комбинировать
    xy_t padding;               // внутреннее смещение текста от краев
    align_t a;

    // вот тут должна быть расширяемость по обеим координатам
    // например limit_char может иметь значение 0, тогда тебе надо не менять size по данной координате
    // и высчитывать его в контексте по размеру ( тоесть в обратную сторону )
} tf_cfg_t;

typedef tf_cfg_t text_field_t;

typedef struct {
    const tf_cfg_t * tfcfg;
    xy_t xy;        // вместо margin и padding сразу результат
    xy_t size;      // вместо limit_char
} tf_ctx_t;

static inline xy_t text_field_size_px(const text_field_t * cfg)
{
    return size_add_padding(lcd_text_size_px(cfg->fcfg, cfg->limit_char), cfg->padding);
}

static inline xy_t text_field_text_pos(const form_t * f, const text_field_t * cfg)
{
    return align_form_pos(f, lcd_text_size_px(cfg->fcfg, cfg->limit_char), cfg->a, cfg->padding);
}

static inline void tf_ctx_calc(tf_ctx_t * ctx, form_t * f, const text_field_t * cfg)
{
    ctx->tfcfg = cfg;
    unsigned scale = cfg->fcfg->scale;
    if (scale == 0) {
        scale = 1;
    }

    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        unsigned char_linear_size = cfg->fcfg->font->size.ca[d];
        unsigned gap = cfg->fcfg->gaps.ca[d];
        if (gap == 0) {
            gap = 1;
            // охуенный план также расширять gaps и scale если там нули
            // и у тебя упрется по одной из координат всегда текст, и другую координату можно будет уменьшить
        }
        unsigned text_linear_size = 0;
        if (cfg->limit_char.ca[d]) {
            // вариант когда мы уменьшаем размер формы
            unsigned text_len = cfg->limit_char.ca[d];
            ctx->size.ca[d] = text_len;
            text_linear_size = (char_linear_size * text_len * scale) + ((text_len - 1) * gap);

            // перезаписываем размер по данной координате в форме
            f->s.ca[d] = text_linear_size + (2 * cfg->padding.ca[d]);

            // align
            if (cfg->a.ca[d].center) {
                ctx->xy.ca[d] = (f->s.ca[d] - text_linear_size) / 2;
            } else {
                if (cfg->a.ca[d].edge == EDGE_L) {
                    ctx->xy.ca[d] = cfg->padding.ca[d];
                } else {
                    ctx->xy.ca[d] = f->s.ca[d] - cfg->padding.ca[d] - text_linear_size;
                }
            }
        } else {
            // вариант когда мы расширяем по максимуму
            unsigned available = f->s.ca[d] - (2 * cfg->padding.ca[d]);
            ctx->size.ca[d] = (available + gap) / ((char_linear_size * scale) + gap);

            // align
            ctx->xy.ca[d] = cfg->padding.ca[d];
        }
    }

    // ctx->xy = align_form_pos(f,
    
    //  text_field_text_pos(f, cfg);


    /*
        задача в имеющуюся родительскую форму вписать какойто произвольный конфиг обьектов

            обьект эт прямоугольник, тоесть form, он может по каждой координате:
            занять все доступоное пространство - как двум таким умникам поделить одно пространство ?
            иметь какойто минимум, зависимость от текста который мы хотим в нем расположить

        я хочу заебашить чтобы у меня считалась форма сама по конфигу, и если там в лимитах 0 по кординате то она не меняется
            либо можно передавать в размеры родительской формы

            опять таки надо координаты

        например часы - там точно 5 символов и тебе надо ебануть это по центру, либо склеить часы из 5 символов с другим полем в котором только секунды чуть меньше размероми все это центрировать
    */

}



/*

итак значит мне сейчас надо сейчас нахуевертить механизм отрисовки чегонибудь

у тебя есть текстовое поле и к нему массив структур, количество структур.

даже несколько массивов. статический и динамический

для статической надписи тебе надо либо указатель на текст, либо на указатель по индексу

для динамической надписи тебе надо указатель на функцию которая преобразует значение в текст

*/

struct label_list;

typedef struct {
    union {
        const char * text;
        const char * (*to_str)(unsigned x);
        const struct label_list * sl;
        void (*vt_by_idx)(val_text_t * vt, unsigned idx);
        val_text_t vt;
        const char ** text_list;
        // const char * (*to_str_val)(void * p, val_rep_t r);
    };
    xy_t xy;
    union {
        uint16_t len;
        uint16_t sofs;      // sub label ctx offset in ctx
    };
    uint16_t ofs;           // val offset in ctx
    val_rep_t rep;          // val representation in ctx
    enum {
        LP_S,               // sub label
        LP_V,               // value to str, format from const vt
        LP_V_FIDX,          // value to str, format from function by index
        LP_T,               // text from const pointer
        LP_T_FIDX,          // text from to_str function by index
        LP_T_FV,            // text from to_str function by unsigned val
        LP_T_LIDX,          // text from pointers array list by index
        LP_T_LV,            // text from pointers array list by unsigned val

        LV = LP_V,
        LVFI = LP_V_FIDX,
        LF = LP_T_FV,
        LS = LP_S,
    } t : 4;
} label_t;

struct label_list {
    void (*ctx_update)(void * ctx, unsigned idx);
    union {
        const label_t * list;
        const void * wrap_list;     // может быть какойто другой тип. на усмотрение реализации печатающей функции.
    };
    uint8_t count;
};

typedef struct label_list label_list_t;
