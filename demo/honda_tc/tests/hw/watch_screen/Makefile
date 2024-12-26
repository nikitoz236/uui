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
SRC += $(LIB)/common/date_time.c

# rtc.h
INC += $(LIB)/hw/include/

# widget_test_rect_with_text.h
INC += $(LIB)/graphic/ui_tree/widgets
SRC += $(LIB)/graphic/ui_tree/widgets/widget_test_rect_with_text.c

# ui_tree.h
INC += $(LIB)/graphic/ui_tree
SRC += $(LIB)/graphic/ui_tree/ui_tree.c

# debug_print.h
INC += $(LIB)/emu

# fonts.h
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/fonts.c
SRC += $(LIB)/graphic/fonts/font_5x7.c

# lcd_text_color.h
INC += $(LIB)/graphic/lcd_color
# SRC += $(LIB)/graphic/lcd_color/*.c
SRC += $(LIB)/graphic/lcd_color/lcd_text_color.c

# forms_edge.h
INC += $(LIB)/graphic/forms


INC += $(LIB)/graphic
SRC += $(LIB)/graphic/lcd_text.c
SRC += $(LIB)/graphic/align_forms.c


# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
