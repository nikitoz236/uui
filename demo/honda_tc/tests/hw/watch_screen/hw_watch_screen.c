#include "config.h"
#include "rtc.h"
#include "array_size.h"
#include "systick.h"
#include "lcd_spi.h"
#include "api_lcd_color.h"

#include "ui_tree.h"

#include "widget_watch_screen.h"
#include "widget_selectable_list.h"
#include "widget_screen_switch.h"
#include "widget_time_settings.h"
#include "widget_text.h"

#include "mstimer.h"

#include "tc_colors.h"

#include "storage.h"

/*
    конфигурация логанализатора
    CH1 - USART_TX  PA9
    CH2 - LED       PC13
    CH3 - CAN_RX    PB8
    CH4 - CAN_TX    PB9
    CH5 - LCD_DC    PA11
    CH6 - LCD_SCL   PA5
    CH7 - LCD_SDO   PA6
    CH8 - LCD_SDI   PA7

 */

const gpio_pin_t debug_gpio_list[] = {
    { GPIO_PORT_C, 13 },
    { GPIO_PORT_B, 8 },
    { GPIO_PORT_B, 9 }
};

void __debug_usart_tx_data(const char * s, unsigned len) { (void)s; (void)len; };

uint8_t ui_ctx[1024] = {0};

#include "btn_list.h"
extern const gpio_pin_t btn_list[];
extern const gpio_cfg_t btn_cfg;

void init_buttons(void)
{
    for (unsigned i = 0; i < BTN_COUNT; i++) {
        gpio_set_cfg(&btn_list[i], &btn_cfg);
    }
}

mstimer_t btn_dead_timer = { .timeout = 50 };

unsigned btn_last = 0;
unsigned btn_last_ret = 0;

unsigned btn_get_event(void)
{
    unsigned btn_state = 0;
    for (unsigned i = 0; i < BTN_COUNT; i++) {
        if (gpio_get_state(&btn_list[i])) {
            btn_state |= 1 << i;
        }
    }
    if (btn_last != btn_state) {
        btn_last = btn_state;
        mstimer_reset(&btn_dead_timer);
    }

    if (mstimer_is_over(&btn_dead_timer)) {
        if (btn_last_ret != btn_last) {
            btn_last_ret = btn_last;
            return btn_last;
        }
    }

    return 0;
}

#define TIMEZONE_FILE_ID            0x001A

int timezone_s = 0;

int time_zone_get(void)
{
    return timezone_s;
}

void time_zone_set(int tz)
{
    timezone_s = tz;
    storage_write_file(TIMEZONE_FILE_ID, &timezone_s, sizeof(tz));
}

#define COLOR_BG_UNSELECTED         COLOR(0x4d3143)
#define COLOR_BG_SELECTED           COLOR(0x966bfa)
#define COLOR_TEXT                  COLOR(0xfcba03)
#define COLOR_FG_SELECTED           COLOR(0xff3334)

const lcd_color_t tc_colors[] = {
    [TC_COLOR_BG_UNSELECTED] = COLOR_BG_UNSELECTED,
    [TC_COLOR_FG_UNSELECTED] = COLOR_TEXT,
    [TC_COLOR_BG_SELECTED] = COLOR_BG_SELECTED,
    [TC_COLOR_FG_SELECTED] = COLOR_FG_SELECTED
};

uint8_t screen_selector = 0;

extern font_t font_5x7;

ui_node_desc_t ui = {
    .widget = &__widget_screen_switch,
    .cfg = &(__widget_screen_switch_cfg_t){
        .selector_ptr = &screen_selector,
        .screens_num = 2,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &__widget_watch_screen,
            },
            {
                .widget = &__widget_selectable_list,
                .cfg = &(__widget_selectable_list_cfg_t) {
                    .num = 5,
                    .margin = { .x = 4, .y = 4 },
                    .different_nodes = 0,
                    .ui_node = (ui_node_desc_t[]) {
                        {
                            .widget = &__widget_time_settings
                        },
                        {
                            .widget = &__widget_date_settings
                        },
                        {
                            .widget = &__widget_time_zone_settings
                        },
                        {
                            .widget = &__widget_text,
                            .cfg = &(widget_text_cfg_t){
                                .text = "one",
                                .color_bg_selected = COLOR_BG_SELECTED,
                                .color_bg_unselected = COLOR_BG_UNSELECTED,
                                .color_text = COLOR_TEXT,
                                .font = &font_5x7,
                                .scale = 4
                            }
                        },
                        {
                            .widget = &__widget_text,
                            .cfg = &(widget_text_cfg_t){
                                .text = "two",
                                .color_bg_selected = COLOR_BG_SELECTED,
                                .color_bg_unselected = COLOR_BG_UNSELECTED,
                                .color_text = COLOR_TEXT,
                                .font = &font_5x7,
                                .scale = 4
                            }
                        },
                    }
                }
            }
        }
    }
};


int main(void)
{
    //  000 Zero wait state, if 0 < SYSCLK≤ 24 MHz
    //  001 One wait state, if 24 MHz < SYSCLK ≤ 48 MHz
    //  010 Two wait states, if 48 MHz < SYSCLK ≤ 72 MHz
    FLASH->ACR |= FLASH_ACR_LATENCY * 2;
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    hw_rcc_apply_cfg(&hw_rcc_cfg);

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    const hw_pclk_t dma_pclk = {
        .mask = RCC_AHBENR_DMA1EN,
        .bus = PCLK_BUS_AHB
    };

    hw_rcc_pclk_ctrl(&dma_pclk, 1);
    init_systick();
    init_rtc();

    storage_init();
    unsigned tz_len;
    int * tz_file = storage_search_file(TIMEZONE_FILE_ID, &tz_len);
    if (tz_file) {
        timezone_s = *tz_file;
    }

    __enable_irq();

    init_lcd_hw(&lcd_cfg);
    lcd_bl(4);
    init_lcd(&lcd_cfg);

    ui_tree_init(ui_ctx, 1024, &ui, &(xy_t){ .w = 320, .h = 240});
    ui_tree_draw();

    while (1) {
        ui_tree_process(btn_get_event());
    };

    return 0;
}
