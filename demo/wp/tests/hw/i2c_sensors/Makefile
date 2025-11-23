LIB = ../../../../../lib

BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = gd32e23x_mboot.ld
STACK_SIZE = 512
OPT = -Os
# DEF += STM32F030

TARGET = test_i2c

INC += .
INC += $(LIB)/common/
INC += $(LIB)/hw/uc_hw/
INC += $(LIB)/hw/uc_hw/gd32e230

SRC += $(TARGET).c

SRC += $(LIB)/hw/uc_hw/gd32e230/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/gd32e230/periph_gpio.c
SRC += $(LIB)/hw/uc_hw/gd32e230/periph_i2c.c

SRC += ../mboot.c
include ../mboot.mk
include $(LIB)/hw/system/uc_build.mk
