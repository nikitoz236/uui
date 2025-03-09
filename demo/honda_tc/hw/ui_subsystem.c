#include "config.h"

#include "ui_tree.h"
#include "mstimer.h"

#include "widget_screen_switch.h"
#include "widget_menu_screen.h"

#include "widget_text.h"

#define DP_NAME "UI"
#include "dp.h"


extern font_t font_5x7;

static const tf_cfg_t tf = {
    .fcfg = &(lcd_font_cfg_t){
        .font = &font_5x7,
        .gaps = { .x = 2, .y = 2 },
        .scale = 2
    },
    .limit_char = { .y = 1 },
    .padding = { .x = 2, .y = 2 },
    .a = ALIGN_LIC,
};

static const color_scheme_t bg_list[] = {
    COLOR(0x654321),
    COLOR(0x123456),
};

static const label_color_t labels = {
    .color = COLOR(0xAB5612),
    .l = {
        .t = LP_T_LIDX,
        .text_list = (const char * []){
            "Hello, world!",
        }
    }
};

ui_node_desc_t ui = {
    .widget = &widget_text,
    .cfg = &(widget_text_cfg_t) {
        .tf_cfg = &tf,
        .label = &labels,
        .bg = bg_list,
    }
};

// ui_node_desc_t ui = {
//     .widget = &widget_screen_switch,
//     .cfg = &(widget_screen_switch_cfg_t){
//         .screens_num = 4,
//         .screens_list = (ui_node_desc_t[]){
//             {
//                 .widget = &widget_menu_screen,
//             }
//         }
//     }
// };

#define UI_MEM_SIZE 2048
static uint8_t ui_ctx[1024] = {};

static uint8_t lcd_sleep = 0;
static uint8_t display_state = 0;

#define UI_TIMEOUT_DLC      2000
#define UI_TIMEOUT_BTN      8000

static mstimer_t timer = {0};

static void ui_display_ctrl(unsigned state)
{
    if (display_state == state) {
        return;
    }

    display_state = state;
    dp("ui display "); dpd(state, 1); dn();

    if (state) {
        init_lcd(&lcd_cfg);
        ui_tree_draw();
        lcd_bl(4);
    } else {
        lcd_pwr(0);
        lcd_bl(0);
    }
}

void init_ui(void)
{
    ui_tree_init(ui_ctx, 1024, &ui, &(xy_t){ .w = 320, .h = 240});

    dn();
    dpn("init display ...");

    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);

    ui_tree_draw();

    lcd_bl(4);
    display_state = 1;

    lcd_sleep = 1;
    mstimer_start_timeout(&timer, UI_TIMEOUT_DLC);
}

void ui_process(unsigned x)
{
    if (x) {
        mstimer_start_timeout(&timer, UI_TIMEOUT_BTN);
    }

    if (lcd_sleep) {
        if (mstimer_is_over(&timer)) {
            // off
            ui_display_ctrl(0);
        } else {
            if (display_state == 0) {
                dpn("ui wake by button");
                ui_display_ctrl(1);
            } else {
                // ui_tree_process(x);
            }
        }
    } else {
        // ui_tree_process(x);
    }

    /*
    логика
    если выключили длц, то
        если в течени 30 секунд не нажимали кнопки, то
            экран часов, через 10 секунд потухнуть
            иначе потухнуть через 30 секунд не меняя экран
    как включается длц то
        если не был активен
            запускаем экран

    */
}

void ui_set_state(unsigned state)
{
    if (state) {
        lcd_sleep = 0;
        ui_display_ctrl(1);
    } else {
        lcd_sleep = 1;
        if (mstimer_is_over(&timer)) {
            // кнопки не нажимали
            dpn("ui dlc sleep");
            mstimer_start_timeout(&timer, UI_TIMEOUT_DLC);
        }
    }
}
