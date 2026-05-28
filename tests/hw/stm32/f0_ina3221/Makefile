LIB = ../../../../lib

TARGET = test_f0_ina3221
BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = stm32f051x8.ld
STACK_SIZE = 512
OPT = -Os

OPENOCD_TARGET = stm32f0x

DEF += STM32F051

INC += .

SRC += $(TARGET).c

INC += $(LIB)/hw/uc_hw/stm32f0x

SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_i2c.c

INC += $(LIB)/hw/uc_hw/
SRC += $(LIB)/hw/uc_hw/stm_usart.c
SRC += $(LIB)/hw/uc_hw/systick.c

INC += $(LIB)/hw/include

INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/ina3221.c

INC += $(LIB)/common

SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c

include $(LIB)/hw/system/uc_build.mk
