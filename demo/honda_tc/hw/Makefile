PROJECT = ..
LIB = $(PROJECT)/../../lib

TARGET = tc_uc_main
BUILD_DIR = build
CPU = -mcpu=cortex-m3
LDSCRIPT = stm32f103x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32f1x
OPT = -Os

# DEF += STM32F103
DEF += STM32F10X_MD

INC += .
INC += $(LIB)/common

SRC += tc_uc_main.c
SRC += hw_cfg.c

SRC += $(LIB)/hw/uc_hw/stm32f10x_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_usart.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_rtc.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_utils.c

include $(LIB)/hw/system/uc_build.mk
