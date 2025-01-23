LIB = ../../../../lib

TARGET = test_g0_pwm
BUILD_DIR = build
CPU = -mcpu=cortex-m0plus
LDSCRIPT = stm32g030x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32g0x
OPT = -Os -g3

INC += .

SRC += $(TARGET).c

INC += $(LIB)/common
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c

SRC += $(LIB)/hw/uc_hw/stm_rcc_common.c
SRC += $(LIB)/hw/uc_hw/stm_pwm.c
SRC += $(LIB)/hw/uc_hw/stm_usart.c
SRC += $(LIB)/hw/uc_hw/stm32g0x_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f0x_gpio.c

include $(LIB)/hw/system/uc_build.mk
