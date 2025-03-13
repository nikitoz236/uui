/*
    итак я хочу некоторую декларативную схему описания расположения элементов внутри виджета

    например для параметра мне надо:
        название
        значение
        единицы измерения

    для одометра:
        название
        время старта
        дата старта
        параметр топливо
        параметр дистанция
        параметр время в пути
        параметр средний расход
        параметр средняя скорость

        DIST
        TIME
        SINCE
        AVG SPEED
        AVG CONS

    с одной стороны мы можем изначально знать наши максимально доступные размеры ? или не можем
    надо выдать минимум. потом подстроится. хз.

    на стадии calc нам нужно посчитать размер всех декларативно описаных элементов

    дальше надо на стадии extend увеличить то что можно ? scale?
    выровнять по референсным точкам

    не пытайся сделать сразу много, просто ебани минимум чтобы нарисовать компьютер

    варианты
        один виджет на весь экран
        несколько виджетов на экране, несколько экранов

    вообще когда я еду я бы хотел видеть:
        любой из параметров крупно

        текущее время

        температуру двигателя
        давление масла

        температура улице
        температура в салоне

        мгновенный расход

        сколько литров после заправки
        время в пути сегодня
        километров за сегодня
        время в пути с момента остановки или заправки - чтобы делать отдых

    кажется что это просто сетка виджетов, а сами виджеты могут быть разными, как часами так и кучей всего другого

        виджет текущее время:
            просто часы по центру, ну или както в одном стиле

        виджет параметр значение:
            название + значение + единицы измерения, какойто layout уже

        виджет маршрутный параметр:
            название маршрута + название параметра + значение + единицы измерения

    список параметров
        без выбора пока сделать просто list, там сразу сырое значение пусть будет и пересчитаное

    список маршрутов
        такойже список, просто все маршруты компактно

        JOURNEY
        TOTAL
        TRAVEL
        REFILL
        TRIP
        DAY
        TMP


    список поездок


    список заправок


    еще прикол в том что все обьекты это текст с зазорами и привязками.
        у текста есть размер, пусть 2-мерный
        у текста есть отступ и выравнивание, относительно чего ? некой внешней формы ?

*/





/*

что мы тут видим ?
форма поделена пополам - типичный лайаут
внутри формы есть заголовок ? - нууу скорее это форма внутри которой с некоторым отступом выравнено текстовое поле с названием. левый край по центру

правый край по центру - блок
    внутри 2 блока левый по центру и правый по центру, однако нам не интересны маргины, они такиеже как у названия
        оба блока состоят из двух текстовых полей.
            внутри первого оба поля выровнены по левому верхнему и левому нижнему краю
            внутри второго блока поля выровнены по правому верхнему и правому нижнему краю
*/

#include "widget_dash.h"

#include "text_label_color.h"
#include "draw_color.h"
#include "lcd_color.h"

typedef struct {
    unsigned v;
} uv_t;

typedef struct {
    tf_ctx_t tf;
    uv_t uv;
} ctx_t;

extern const font_t font_5x7;
// extern font_t font_5x7;

static const tf_cfg_t tf_cfg_title = {
    .fcfg = &(lcd_font_cfg_t) {
        .font = &font_5x7,
        .scale = 1,
        .gaps = { .x = 1 },
    },
    .limit_char = { .y = 1 },
    .padding = { .x = 0, .y = 2 },
    .a = ALIGN_CUI
};

static const tf_cfg_t tf_cfg_val = {
    .fcfg = &(lcd_font_cfg_t) {
        .font = &font_5x7,
        .scale = 4,
        .gaps = { .x = 2 },
    },
    .limit_char = { .y = 1 },
    .padding = { .x = 0, .y = 2 },
    .a = ALIGN_CDI
};

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    draw_color_form(&el->f, COLOR(0xAABB11));
    printf("draw %d %d %d %d\n", el->f.p.x, el->f.p.y, el->f.s.x, el->f.s.y);

    tf_ctx_t title;
    form_t f = el->f;

    printf("form %d %d %d %d\n", f.p.x, f.p.y, f.s.x, f.s.y);

    tf_ctx_calc(&title, &f, &tf_cfg_title);
    printf("title: %d %d %d %d\n", title.xy.x, title.xy.y, title.size.x, title.size.y);

    lcd_color_tf_print("    ", &title, &(color_scheme_t){ .bg = COLOR(0x334411), .fg = COLOR(0x000000) }, 0, title.size.x);

    f = el->f;
    printf("form %d %d %d %d\n", f.p.x, f.p.y, f.s.x, f.s.y);

    printf("pf %d %d %d %d\n", el->f.p.x, el->f.p.y, el->f.s.x, el->f.s.y);
    tf_ctx_calc(&ctx->tf, &f, &tf_cfg_val);
    printf("val: %d %d %d %d\n", ctx->tf.xy.x, ctx->tf.xy.y, ctx->tf.size.x, ctx->tf.size.y);
    lcd_color_tf_print("    ", &ctx->tf, &(color_scheme_t){ .bg = COLOR(0x334411), .fg = COLOR(0x000000) }, 0, ctx->tf.size.x);
}

static void update(ui_element_t * el)
{
    printf("update\n");
}

const widget_desc_t widget_dash_item = {
    .draw = draw,
    .update = update,
    .ctx_size = sizeof(ctx_t),
};

void form_grid_by_coord(const form_t * p, form_t * f, xy_t size, xy_t margin, xy_t coord)
{
    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        f->s.ca[d] = ((p->s.ca[d] + margin.ca[d]) / size.ca[d]) - margin.ca[d];
        f->p.ca[d] = p->p.ca[d] + ((f->s.ca[d] + margin.ca[d]) * coord.ca[d]);
    }
}

unsigned form_grid_by_idx(const form_t * p, form_t * f, xy_t size, xy_t margin, unsigned idx)
{
    xy_t coord;
    coord.y = idx / size.x;
    if (coord.y >= size.y) {
        return 0;
    }
    coord.x = idx % size.x;
    form_grid_by_coord(p, f, size, margin, coord);
    return 1;
}

static void draw_dash(ui_element_t * el)
{
    unsigned idx = 0;
    form_t f;
    while (form_grid_by_idx(&el->f, &f, (xy_t){ .x = 2, .y = 5 }, (xy_t){ .x = 2, .y = 2 }, idx)) {
        ui_element_t * item = ui_tree_add(el, &(ui_node_desc_t){ .widget = &widget_dash_item }, idx);
        item->f = f;
        ui_tree_element_draw(item);
        idx++;
    }
}

const widget_desc_t widget_dash = {
    .draw = draw_dash,
};
