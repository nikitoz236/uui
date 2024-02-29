#include "widget_metric_list_item.h"
#include "lcd_text_color.h"
extern font_t font_5x7;

typedef struct {
    lcd_text_cfg_t text_cfg;
} __widget_metric_list_item_ctx_t;

static void calc(ui_element_t * el)
{
    __widget_metric_list_item_cfg_t * cfg = (__widget_metric_list_item_cfg_t *)el->ui_node->cfg;
    __widget_metric_list_item_ctx_t * ctx = (__widget_metric_list_item_ctx_t *)el->ctx;
    ctx->text_cfg.font = &font_5x7;
    ctx->text_cfg.scale = 1;
    ctx->text_cfg.gaps.x = 1;
    ctx->text_cfg.gaps.y = 0;
    ctx->text_cfg.text_size.x = sizeof("metric name 0123456 units 0000");
    ctx->text_cfg.text_size.y = 1;
    lcd_text_calc_size(&el->f.s, &ctx->text_cfg);
}

static void update(ui_element_t * el)
{
    __widget_metric_list_item_cfg_t * cfg = (__widget_metric_list_item_cfg_t *)el->ui_node->cfg;
    __widget_metric_list_item_ctx_t * ctx = (__widget_metric_list_item_ctx_t *)el->ctx;
}

static void draw(ui_element_t * el)
{
    __widget_metric_list_item_cfg_t * cfg = (__widget_metric_list_item_cfg_t *)el->ui_node->cfg;
    __widget_metric_list_item_ctx_t * ctx = (__widget_metric_list_item_ctx_t *)el->ctx;
    lcd_text_extend_text_size(&el->f.s, &ctx->text_cfg);
    color_scheme_t cs = {
        .fg = cfg->color_text,
        .bg = cfg->color_bg
    };
    unsigned m_id = el->idx;
    unsigned pos_val = METRIC_NAME_MAX_LEN + 1;
    unsigned pos_unit = pos_val + 10;
    unsigned pos_raw = pos_unit + METRIC_UNIT_MAX_LEN + 1;

    lcd_text_color_print(metric_var_get_name(m_id), &el->f.p, &ctx->text_cfg, &cs, 0, 0, METRIC_NAME_MAX_LEN + 1);

    cs.fg = cfg->color_unit;
    lcd_text_color_print(metric_var_get_unit(m_id), &el->f.p, &ctx->text_cfg, &cs, pos_unit, 0, METRIC_UNIT_MAX_LEN + 1);

    cs.fg = cfg->color_value;
    lcd_text_color_print("0123456", &el->f.p, &ctx->text_cfg, &cs, pos_val, 0, 8);
    cs.fg = cfg->color_raw;
    lcd_text_color_print("55AA", &el->f.p, &ctx->text_cfg, &cs, pos_raw, 0, 4);

}

widget_desc_t __widget_metric_list_item = {
    .calc = calc,
    .draw = draw,
    .update = update,
    .ctx_size = sizeof(__widget_metric_list_item_ctx_t)
};
