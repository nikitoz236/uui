PROJECT = ..
LIB = ../../../../lib

TARGET = ili9341_test
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

INC += $(LIB)/hw/uc_hw
INC += $(LIB)/hw/uc_hw/stm32f1x

SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_spi.c
SRC += $(LIB)/hw/uc_hw/systick.c
SRC += $(LIB)/hw/uc_hw/stm_pwm.c

INC += $(LIB)/hw/api/
INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ili9341.c

INC += $(LIB)/common
# SRC += $(LIB)/common/str_val.c
# SRC += $(LIB)/common/str_utils.c

INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/fonts.c
SRC += $(LIB)/graphic/fonts/font_5x7.c

INC += $(LIB)/graphic
SRC += $(LIB)/graphic/lcd_text.c

INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/lcd_text_color.c

# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
