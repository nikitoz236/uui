#include "config.h"

#include "ui_tree.h"
#include "mstimer.h"

#include "widget_screen_switch.h"
#include "widget_menu_screen.h"

#include "widget_text.h"

#define DP_NAME "UI"
#include "dp.h"


#include "widget_dash.h"


/////////////////////////////////

static const ui_node_desc_t dash = {
    .widget = &widget_screen_switch,
    .cfg = &(widget_screen_switch_cfg_t){
        .screens_num = 2,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &widget_dash
            },
        }
    }
};

static void draw(ui_element_t * el)
{
    ui_element_t * item = ui_tree_add(el, &dash, 0);
    ui_tree_element_draw(item);
    ui_tree_element_select(item, 1);
}

#include "event_list.h"

static unsigned process(ui_element_t * el, unsigned event)
{
    // printf("process %d\n", event);
    if (event == EVENT_BTN_MODE) {
        ui_tree_delete_childs(el);
        ui_element_t * item = ui_tree_add(el, &node_widget_menu_screen, 0);
        ui_tree_element_draw(item);
        ui_tree_element_select(item, 1);
        return 1;
    }
    if (event == EVENT_BTN_BACK) {
        ui_tree_delete_childs(el);
        draw(el);
        return 1;
    }
    return 0;
}

const ui_node_desc_t ui = {
    .widget = &(widget_desc_t) {
        .draw = draw,
        .process_event = process
    }
};

////////////////////////////////////

#define UI_MEM_SIZE 2048
static uint8_t ui_ctx[UI_MEM_SIZE] __attribute__((aligned(4))) = {};

static uint8_t lcd_sleep = 0;
static uint8_t display_state = 0;

#define UI_TIMEOUT_DLC      5000
#define UI_TIMEOUT_BTN      30000

static mstimer_t timer = {0};

unsigned br = 10;

void ui_set_brightness(unsigned br)
{
    dp("ui set br "); dpd(br, 1); dn();
    lcd_bl(br);
}

static void ui_display_ctrl(unsigned state)
{
    if (display_state == state) {
        return;
    }

    display_state = state;
    dp("ui display "); dpd(state, 1); dn();

    if (state) {
        init_lcd(&lcd_cfg);
        // lcd_pwr(1);

        ui_tree_init(ui_ctx, UI_MEM_SIZE, &ui, &(xy_t){ .w = 320, .h = 240});
        ui_tree_draw();

        lcd_bl(br);
    } else {
        lcd_bl(0);
        lcd_pwr(0);
    }
}

void init_ui(void)
{
    ui_tree_init(ui_ctx, UI_MEM_SIZE, &ui, &(xy_t){ .w = 320, .h = 240});

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
                ui_tree_process(x);
            }
        }
    } else {
        ui_tree_process(x);
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
