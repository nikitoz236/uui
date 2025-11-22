LIB = ../../../../../lib

BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = gd32e23x_mboot.ld
STACK_SIZE = 512
OPT = -Os
DEF += STM32F030

TARGET = test_wp_ws

INC += .
INC += $(LIB)/common/
INC += $(LIB)/hw/uc_hw/
INC += $(LIB)/hw/uc_hw/stm32f0x

SRC += $(TARGET).c

SRC += ../../../drivers/stm_ws2812b.c
INC += ../../../drivers/

SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_gpio.c

SRC += $(LIB)/hw/uc_hw/stm_pwm.c

SRC += ../mboot.c
include ../mboot.mk
include $(LIB)/hw/system/uc_build.mk
