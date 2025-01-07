PROJECT = ..
LIB = ../../../../../lib

TARGET = hw_watch_screen
BUILD_DIR = build
CPU = -mcpu=cortex-m3
LDSCRIPT = stm32f103x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32f1x
OPT = -Os

# DEF += STM32F103
DEF += STM32F10X_MD

INC += .

SRC += $(TARGET).c
SRC += hw_cfg.c

# drivers
SRC += $(LIB)/hw/uc_hw/stm32f10x_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_gpio.c
# SRC += $(LIB)/hw/uc_hw/stm32f10x_usart.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_rtc.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_systick.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_pwm.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_spi.c

INC += $(LIB)/hw/api/
INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ili9341.c


# misc.h
INC += $(LIB)/common
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/date_time.c
SRC += $(LIB)/common/val_mod.c


# rtc.h
INC += $(LIB)/hw/include/

# ui_tree.h
INC += $(LIB)/graphic/ui_tree
SRC += $(LIB)/graphic/ui_tree/ui_tree.c

# debug_print.h
INC += $(LIB)/emu

# fonts.h
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/fonts.c
SRC += $(LIB)/graphic/fonts/font_5x7.c
SRC += $(LIB)/graphic/fonts/font_3x5.c

# lcd_text_color.h
INC += $(LIB)/graphic/lcd_color
# SRC += $(LIB)/graphic/lcd_color/*.c
SRC += $(LIB)/graphic/lcd_color/lcd_text_color.c

# forms_edge.h
# forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

INC += $(LIB)/graphic
SRC += $(LIB)/graphic/lcd_text.c
SRC += $(LIB)/graphic/align_forms.c
SRC += $(LIB)/graphic/forms/forms_align.c

# tc_colors.h
INC += ../../../view

# widgets
INC += ../../../widgets
SRC += ../../../widgets/widget_watch_screen.c

SRC += ../../../widgets/widget_time_settings.c
SRC += ../../../widgets/common/widget_text.c


INC += ../../../widgets/common
SRC += ../../../widgets/common/widget_screen_switch.c
SRC += ../../../widgets/common/widget_selectable_list.c

# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk

test_color_convert:
	gcc test_color_convert.c -o test_color_convert && ./test_color_convert && rm ./test_color_convert
