LIB = ../../../../lib

TARGET = hw_test_storage
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
INC += $(LIB)/hw/include/

SRC += $(LIB)/hw/uc_hw/stm32f10x_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_usart.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_flash.c

# storage
INC += $(LIB)/storage
SRC += $(LIB)/storage/storage.c

# misc.h
# dp.h
INC += $(LIB)/common

# dp.h dependecies
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c

# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
