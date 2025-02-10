LIB = ../../../../lib

TARGET = stm32_usart_test
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

SRC += $(LIB)/hw/uc_hw/stm_usart.c
SRC += $(LIB)/hw/uc_hw/systick.c

INC += $(LIB)/hw/drivers

INC += $(LIB)/common
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c

# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
