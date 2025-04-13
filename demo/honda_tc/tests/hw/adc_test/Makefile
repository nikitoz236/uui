PROJECT = ../../..
LIB = $(PROJECT)/../../lib

TARGET = honda_tc_adc_test
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

INC += $(PROJECT)/hw
SRC += $(PROJECT)/hw/adc_hw.c


INC += $(LIB)/hw/uc_hw
SRC += $(LIB)/hw/uc_hw/stm_usart.c

SRC += $(LIB)/hw/uc_hw/systick.c

INC += $(LIB)/common
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val_buf.c

PERIPH = $(LIB)/hw/uc_hw/stm32f1x

INC += $(PERIPH)

SRC += $(PERIPH)/periph_rcc.c
SRC += $(PERIPH)/periph_gpio.c

include $(LIB)/hw/system/uc_build.mk
