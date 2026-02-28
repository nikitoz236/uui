#include "emu_graphic_device.h"
#include "ui_tree.h"

static uint8_t emu_stack[128];
static ui_node_desc_t lcd_node;

static void calc(ui_element_t * node_ctx)
{
    emu_lcd_init(&node_ctx->f, ((emu_lcd_cfg_t *)node_ctx->ui_node->cfg));
}

static void draw(ui_element_t * node_ctx)
{
    emu_lcd_clear();
};

widget_desc_t __widget_emu_lcd = {
    .calc = calc,
    .draw = draw,
};

void emu_graphic_device_init(emu_lcd_cfg_t * cfg)
{
    ui_element_t * lcd_ctx = (ui_element_t *)emu_stack;
    lcd_node.widget = &__widget_emu_lcd;
    lcd_node.cfg = cfg;
    lcd_ctx->ui_node = &lcd_node;

    lcd_ctx->ui_node->widget->calc(lcd_ctx);
    
    lcd_ctx->ui_node->widget->draw(lcd_ctx);
}
