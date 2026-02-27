PROJECT = ..
LIB = ../../../../lib

TARGET = f0_lora_e220
BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = stm32f051x8.ld
STACK_SIZE = 512
OPT = -Os

OPENOCD_TARGET ?= stm32f0x

DEF += STM32F051

INC += .

SRC += $(TARGET).c

PERIPH = $(LIB)/hw/uc_hw/stm32f0x

INC += $(PERIPH)

SRC += $(PERIPH)/periph_rcc.c
SRC += $(PERIPH)/periph_gpio.c
SRC += $(PERIPH)/periph_spi.c

INC += $(PERIPH)/..
SRC += $(PERIPH)/../systick.c
SRC += $(PERIPH)/../stm_usart.c

INC += $(LIB)/common
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c

INC += $(LIB)/hw/include
INC += $(LIB)/hw/drivers

SRC += $(LIB)/hw/drivers/sx1262.c
SRC += $(LIB)/hw/drivers/lora.c

include $(LIB)/hw/system/uc_build.mk
