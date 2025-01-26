LIB = ../../../../lib

TARGET = test_f1_sd_card
BUILD_DIR = build
CPU = -mcpu=cortex-m3
LDSCRIPT = stm32f103x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32f1x
OPT = -Os

DEF += STM32F10X_MD

INC += .

SRC += $(TARGET).c

PERIPH = $(LIB)/hw/uc_hw/stm32f1x

INC += $(PERIPH)

SRC += $(PERIPH)/periph_rcc.c
SRC += $(PERIPH)/periph_gpio.c
SRC += $(PERIPH)/periph_spi.c
SRC += $(PERIPH)/../stm_usart.c

INC += $(LIB)/common
INC += $(LIB)/hw/uc_hw

SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c

INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/sd_card.c

include $(LIB)/hw/system/uc_build.mk
