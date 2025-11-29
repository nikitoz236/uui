LIB = ../../../../lib

TARGET = test_f1_btn

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

INC += $(LIB)/specific
SRC += $(LIB)/specific/kbd.c

INC += $(LIB)/hw/uc_hw
SRC += $(LIB)/hw/uc_hw/stm_usart.c
SRC += $(LIB)/hw/uc_hw/systick.c

PERIPH = stm32f1x
INC += $(LIB)/hw/uc_hw/$(PERIPH)
SRC += $(LIB)/hw/uc_hw/$(PERIPH)/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/$(PERIPH)/periph_gpio.c

INC += $(LIB)/common
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val_buf.c

include $(LIB)/hw/system/uc_build.mk
