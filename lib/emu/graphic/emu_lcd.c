#include "emu_common.h"
#include "emu_lcd.h"

static uint8_t emu_stack[128];
static ui_node_desc_t lcd_node;

void emu_lcd_init(__widget_emu_lcd_cfg_t * cfg)
{
    ui_element_t * lcd_ctx = (ui_element_t *)emu_stack;
    lcd_node.widget = &__widget_emu_lcd;
    lcd_node.cfg = cfg;
    lcd_ctx->ui_node = &lcd_node;

    lcd_ctx->ui_node->widget->calc(lcd_ctx);
    gfx_open(lcd_ctx->f.s.w, lcd_ctx->f.s.h, "emu lcd");
    lcd_ctx->ui_node->widget->draw(lcd_ctx);
}

void emu_lcd_loop(void (*process)(char key))
{
    while (1) {
        char key = gfx_routine();
        if (key == 'q') {
            break;
        }
        if (process) {
            process(key);
        }

        usleep(100000);
    }
}
