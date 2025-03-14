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


/*
    уже колхоз



*/

#include "widget_dash.h"

#include "text_label_color.h"
#include "draw_color.h"
#include "lcd_color.h"

#include "metrics_view.h"
#include "routes.h"

#include "widget_menu_screen.h"

typedef struct {
    unsigned h;
    uint8_t s;
    uint8_t m;
} route_time_t;

static void ctx_update_route_time(route_time_t * rt, unsigned time_s)
{
    rt->s = time_s % 60;
    time_s /= 60;
    rt->m = time_s % 60;
    rt->h = time_s / 60;
}

typedef struct {
    union {
        int v;
        unsigned uval;
    };
    route_time_t rt;
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
    .padding = { .x = 4, .y = 4 },
    .a = ALIGN_CUI
};

static const tf_cfg_t tf_cfg_val = {
    .fcfg = &(lcd_font_cfg_t) {
        .font = &font_5x7,
        .scale = 4,
        .gaps = { .x = 3 },
    },
    .limit_char = { .y = 1 },
    .padding = { .x = 0, .y = 4 },
    .a = ALIGN_CDI
};

static const tf_cfg_t tf_cfg_route = {
    .fcfg = &(lcd_font_cfg_t) {
        .font = &font_5x7,
        .scale = 3,
        .gaps = { .x = 2 },
    },
    .limit_char = { .y = 1 },
    .padding = { .x = 0, .y = 4 },
    .a = ALIGN_CDI
};

typedef struct {
    enum { DASH_METRIC, DASH_ROUTE, DASH_TIME, DASH_BOOL } type : 8;
    uint8_t route_type;
    uint8_t idx;
} dash_item_t;

static const dash_item_t items[] = {
    { .type = DASH_TIME },
    { .type = DASH_METRIC, .idx = METRIC_ID_INJECTION },

    { .type = DASH_METRIC, .idx = METRIC_ID_ENGINE_T },
    { .type = DASH_METRIC, .idx = METRIC_ID_INJECTOR_LOAD },

    { .type = DASH_METRIC, .idx = METRIC_ID_INTAKE_T },
    { .type = DASH_METRIC, .idx = METRIC_ID_CONS_MOMENT_DIST },

    { .type = DASH_METRIC, .idx = METRIC_ID_VOLTAGE_ADC },
    { .type = DASH_ROUTE,  .idx = ROUTE_VALUE_TIME, .route_type = ROUTE_TYPE_TRIP },

    { .type = DASH_ROUTE,  .idx = ROUTE_VALUE_FUEL, .route_type = ROUTE_TYPE_REFILL },
    { .type = DASH_ROUTE,  .idx = ROUTE_VALUE_DIST, .route_type = ROUTE_TYPE_TRIP },



    { .type = DASH_METRIC, .idx = METRIC_ID_INJECTOR_LOAD },
    { .type = DASH_TIME },
    { .type = DASH_METRIC, .idx = METRIC_ID_INJECTION },

    { .type = DASH_METRIC, .idx = METRIC_ID_ENGINE_T },

    { .type = DASH_METRIC, .idx = METRIC_ID_INTAKE_T },
    { .type = DASH_METRIC, .idx = METRIC_ID_CONS_MOMENT_DIST },

    { .type = DASH_METRIC, .idx = METRIC_ID_VOLTAGE_ADC },
    { .type = DASH_ROUTE,  .idx = ROUTE_VALUE_TIME, .route_type = ROUTE_TYPE_TRIP },

    { .type = DASH_ROUTE,  .idx = ROUTE_VALUE_FUEL, .route_type = ROUTE_TYPE_REFILL },
    { .type = DASH_ROUTE,  .idx = ROUTE_VALUE_DIST, .route_type = ROUTE_TYPE_TRIP },
};

const char * dash_item_name(unsigned idx)
{
    const dash_item_t * item = &items[idx];
    switch (item->type) {
        case DASH_METRIC:
            return metric_var_get_name(item->idx);
        case DASH_ROUTE:
            // меняем местами тип маршрута и имя переменной
            return route_name(item->route_type);
        case DASH_TIME:
            return 0;
        case DASH_BOOL:
            return metric_bool_get_name(item->idx);
    }
    return 0;
}

struct route_val_cfg {
    const char * name;
    const char * unit;
    val_text_t vt;
};

static const struct route_val_cfg route_val_cfg[] = {
    [ROUTE_VALUE_DIST]          = { .name = "DIST",         .unit = "KM", .vt = { .f = X1000, .p = 3, .zr = 1 } },
    [ROUTE_VALUE_FUEL]          = { .name = "FUEL",         .unit = "L", .vt = { .f = X1000, .p = 3, .zr = 1 }  },
    [ROUTE_VALUE_TIME]          = { .name = "TIME", },
    [ROUTE_VALUE_SINCE_TIME]    = { .name = "SINCE", },
    [ROUTE_VALUE_SINCE_ODO]     = { .name = "SINCE",        .unit = "KM", .vt = { .f = X1000, .p = 3, .zr = 1 } },
    [ROUTE_VALUE_AVG_SPEED]     = { .name = "AVG SPEED",    .unit = "KM/H", .vt = { .f = X1000, .p = 2, .zr = 0 } },
    [ROUTE_VALUE_CONS_TIME]     = { .name = "CONS",         .unit = "L / H", .vt = { .f = X1000, .p = 2, .zr = 1 } },
    [ROUTE_VALUE_CONS_DIST]     = { .name = "CONS",         .unit = "L / 100", .vt = { .f = X1000, .p = 2, .zr = 1 } },
};

const char * dash_item_unit(unsigned idx)
{
    const dash_item_t * item = &items[idx];

    switch (item->type) {
        case DASH_METRIC:
            return metric_var_get_unit(item->idx);
        case DASH_ROUTE:
            return route_val_cfg[item->idx].unit;
        case DASH_TIME:
            return 0;
        case DASH_BOOL:
            return 0;
    }
    return 0;
}

const char * dash_item_route_type(unsigned idx)
{
    const dash_item_t * item = &items[idx];
    if(item->type != DASH_ROUTE) {
        return 0;
    }
    return route_val_cfg[item->idx].name;
}

#define COLOR_FG        COLOR(0xE76a35)
#define COLOR_BG        COLOR(0x3A0D12)

static const label_list_t ll_static = {
    .count = 3,
    .wrap_list = (label_color_t[]){
        { .color = COLOR_FG, .l = { .to_str = dash_item_name, .t = LP_T_FIDX, .xy = { .x = 0 }, .len = 12, } },
        { .color = COLOR_FG, .l = { .to_str = dash_item_unit, .t = LP_T_FIDX, .xy = { .x = -6 + 2 }, .len = 6, } },
        { .color = COLOR_FG, .l = { .to_str = dash_item_route_type, .t = LP_T_FIDX, .xy = { .x = 9 }, .len = 8, } },
    },
};

static void val_ctx_update(uv_t * uv, unsigned idx)
{
    const dash_item_t * item = &items[idx];
    switch (item->type) {
        case DASH_METRIC:
            uv->v = metric_var_get_real(item->idx);
            break;
        case DASH_ROUTE:
            uv->uval = route_get_value(item->route_type, item->idx);
            break;
        default:
            uv->v = 0;
            break;
    }
}

static void dash_vt(val_text_t * vt, unsigned idx)
{
    const dash_item_t * item = &items[idx];
    unsigned point = 0;
    dec_factor_t factor = 0;
    switch (item->type) {
        case DASH_METRIC:
            metric_var_get_factor_point(item->idx, &factor, &point);
            vt->f = factor;
            vt->p = point;
            vt->zr = 1;
            vt->zl = 0;
            break;
        case DASH_ROUTE:
            *vt = route_val_cfg[item->idx].vt;
            break;
        case DASH_TIME:
            break;
        case DASH_BOOL:
            break;
    }
}

static const label_list_t ll_val = {
    .count = 1,
    .ctx_update = (void(*)(void *, unsigned))val_ctx_update,
    .wrap_list = (label_color_t[]) {
        { .color = COLOR_FG, .l = { .len = 7, .rep = { .s = 1, .vs = VAL_SIZE_32 }, .t = LP_V_FIDX, .vt_by_idx = dash_vt, .ofs = offsetof(uv_t, v) } },
    },
};

static const label_list_t ll_route = {
    .count = 1,
    .ctx_update = (void(*)(void *, unsigned))val_ctx_update,
    .wrap_list = (label_color_t[]) {
        { .color = COLOR_FG, .l = { .len = 9, .rep = { .vs = VAL_SIZE_32 }, .t = LP_V_FIDX, .vt_by_idx = dash_vt, .ofs = offsetof(uv_t, uval) } },
    },
};

static const label_list_t ll_route_time = {
    .count = 1,
    .ctx_update = (void(*)(void *, unsigned))val_ctx_update,
    .wrap_list = (label_color_t []) { { .color = COLOR_FG, .l = { .t = LP_S, .ofs = offsetof(uv_t, uval), .rep = { .vs = VAL_SIZE_32 }, .sofs = offsetof(uv_t, rt), .sl = &(const label_list_t) {
        .ctx_update = (void(*)(void *, unsigned))ctx_update_route_time, .count = 3, .list = (label_t []) {
            { .len = 3, .t = LP_V, .xy = { .x = 0, }, .rep = { .vs = VAL_SIZE_32 }, .vt = { .zl = 0 }, .ofs = offsetof(route_time_t, h) },
            { .len = 2, .t = LP_V, .xy = { .x = 4, }, .rep = { .vs = VAL_SIZE_8  }, .vt = { .zl = 1 }, .ofs = offsetof(route_time_t, m) },
            { .len = 2, .t = LP_V, .xy = { .x = 7, }, .rep = { .vs = VAL_SIZE_8  }, .vt = { .zl = 1 }, .ofs = offsetof(route_time_t, s) },
        } } } },
    }
};

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;

    draw_color_form(&el->f, COLOR_BG);
    tf_ctx_t title;
    form_t f = el->f;

    tf_ctx_calc(&title, &f, &tf_cfg_title);
    label_color_list(&title, &ll_static, COLOR_BG, el->idx, 0, 0);
    // lcd_color_text_raw_print(0, title.tfcfg->fcfg, &(color_scheme_t) { .bg = COLOR_FG, .fg = COLOR_FG }, &title.xy, &title.size, 0, title.size.x);

    f = el->f;
    if (items[el->idx].type == DASH_ROUTE) {
        tf_ctx_calc(&ctx->tf, &f, &tf_cfg_route);
        if (items[el->idx].idx == ROUTE_VALUE_TIME) {
            lcd_color_text_raw_print(":", ctx->tf.tfcfg->fcfg, &(color_scheme_t) { .bg = COLOR_BG, .fg = COLOR_FG }, &ctx->tf.xy, &title.size, &(xy_t){ .x = 3 }, 1);
            lcd_color_text_raw_print(":", ctx->tf.tfcfg->fcfg, &(color_scheme_t) { .bg = COLOR_BG, .fg = COLOR_FG }, &ctx->tf.xy, &title.size, &(xy_t){ .x = 6 }, 1);
            label_color_list(&ctx->tf, &ll_route_time, COLOR_BG, el->idx, &ctx->uv, 0);
        } else {
            label_color_list(&ctx->tf, &ll_route, COLOR_BG, el->idx, &ctx->uv, 0);
        }
    } else {
        tf_ctx_calc(&ctx->tf, &f, &tf_cfg_val);
        label_color_list(&ctx->tf, &ll_val, COLOR_BG, el->idx, &ctx->uv, 0);
    }

    // printf("size %d\n", ctx->tf.size.x);

    el->drawed = 1;
}

static void update(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    uv_t uv;
    if (items[el->idx].type == DASH_ROUTE) {
        if (items[el->idx].idx == ROUTE_VALUE_TIME) {
            label_color_list(&ctx->tf, &ll_route_time, COLOR_BG, el->idx, &uv, &ctx->uv);
        } else {
            label_color_list(&ctx->tf, &ll_route, COLOR_BG, el->idx, &uv, &ctx->uv);
        }

    } else {
        label_color_list(&ctx->tf, &ll_val, COLOR_BG, el->idx, &uv, &ctx->uv);
    }
}

static const ui_node_desc_t node_widget_dash_item = {
    .widget = &(const widget_desc_t){
        .draw = draw,
        .update = update,
        .ctx_size = sizeof(ctx_t),
    }
};

extern const ui_node_desc_t node_widget_dash_watch;

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

static const xy_t dash_size = { .x = 2, .y = 5 };

static void draw_dash(ui_element_t * el)
{
    unsigned idx = 0;
    form_t f;
    unsigned offset = dash_size.x * dash_size.y;

    draw_color_form(&el->f, 0);
    offset *= el->idx;
    while (form_grid_by_idx(&el->f, &f, dash_size, (xy_t){ .x = 2, .y = 2 }, idx)) {
        const ui_node_desc_t * node = &node_widget_dash_item;
        if (items[offset + idx].type == DASH_TIME) {
            node = &node_widget_dash_watch;
        }
        ui_element_t * item = ui_tree_add(el, node, offset + idx);
        item->f = f;
        ui_tree_element_draw(item);
        idx++;
    }
}

const widget_desc_t widget_dash = {
    .draw = draw_dash,
};
