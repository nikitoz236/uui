PROJECT = ..
LIB = ../../../../../lib

TARGET = dlc_hw_emu
BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = stm32f051x8.ld
STACK_SIZE = 512
OPT = -Os

OPENOCD_TARGET = stm32f0x
OOCD_INTERFACE_FILE = ./second_stlink.cfg

DEF += STM32F051

INC += .

SRC += $(TARGET).c
SRC += dlc_parse.c

INC += ../../../logic
SRC += ../../../logic/honda_dlc_units.c

INC += $(LIB)/hw/uc_hw/stm32f0x

SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_spi.c

INC += $(LIB)/hw/uc_hw/
SRC += $(LIB)/hw/uc_hw/stm_pwm.c
SRC += $(LIB)/hw/uc_hw/stm_usart.c
SRC += $(LIB)/hw/uc_hw/systick.c

INC += $(LIB)/hw/include

INC += $(LIB)/common

SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c

include $(LIB)/hw/system/uc_build.mk
