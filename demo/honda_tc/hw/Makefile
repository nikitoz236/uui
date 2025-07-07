PROJECT = ..
LIB = $(PROJECT)/../../lib

TARGET = tc_uc_main
BUILD_DIR = build
CPU = -mcpu=cortex-m3
LDSCRIPT = stm32f103x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32f1x
OPT = -Os

DEF += STM32F10X_MD


INC += .
SRC += $(TARGET).c
SRC += hw_cfg.c
SRC += kline.c
SRC += time_subsystem.c
SRC += sound_subsystem.c
SRC += ui_subsystem.c
SRC += button_subsystem.c
SRC += adc_hw.c

# logic
INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/dlc_poll.c
SRC += $(PROJECT)/logic/honda_dlc_units.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/metrics_adc.c
SRC += $(PROJECT)/logic/metrics_min_voltage.c
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/routes.c
SRC += $(PROJECT)/logic/trip_integrate.c

# hw drivers
INC += $(LIB)/hw/drivers
INC += $(LIB)/hw/api/

INC += $(LIB)/hw/uc_hw
INC += $(LIB)/hw/uc_hw/stm32f1x

SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_spi.c

SRC += $(LIB)/hw/uc_hw/systick.c
SRC += $(LIB)/hw/uc_hw/stm_usart.c
SRC += $(LIB)/hw/uc_hw/stm_pwm.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_rtc.c

SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ili9341.c

# common
INC += $(LIB)/common
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/date_time.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/val_mod.c

# storage
INC += $(LIB)/storage
SRC += $(LIB)/storage/storage.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_flash.c

# ui
# ==========================================================================

# widgets
INC += $(PROJECT)/widgets/common/
SRC += $(PROJECT)/widgets/common/widget_selectable_list.c
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c
# SRC += $(PROJECT)/widgets/common/widget_text.c

INC += $(PROJECT)/widgets
SRC += $(PROJECT)/widgets/widget_menu_screen.c

SRC += $(PROJECT)/widgets/widget_metric_list_item.c
SRC += $(PROJECT)/widgets/widget_route_list_item.c
SRC += $(PROJECT)/widgets/widget_dlc_dump.c

SRC += $(PROJECT)/widgets/widget_settings_title.c
SRC += $(PROJECT)/widgets/widget_time_settings.c
SRC += $(PROJECT)/widgets/widget_odo_settings.c
SRC += $(PROJECT)/widgets/widget_br_settings.c

INC += $(PROJECT)/ui

INC += $(PROJECT)/tests/graphic/dash
SRC += $(PROJECT)/tests/graphic/dash/widget_dash.c
SRC += $(PROJECT)/tests/graphic/dash/node_widget_dash_watch.c


# forms_edge.h
# forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

INC += $(LIB)/graphic
INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/lcd_text_color.c

# text_field.h
SRC += $(LIB)/graphic/lcd_text.c
INC += $(LIB)/graphic/text
SRC += $(LIB)/graphic/text/text_label_color.c
SRC += $(LIB)/graphic/text/text_field.c

# ui_tree.h
INC += $(LIB)/graphic/ui_tree
SRC += $(LIB)/graphic/ui_tree/ui_tree.c

# fonts.h
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/fonts.c
SRC += $(LIB)/graphic/fonts/font_5x7.c
SRC += $(LIB)/graphic/fonts/font_3x5.c

# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
