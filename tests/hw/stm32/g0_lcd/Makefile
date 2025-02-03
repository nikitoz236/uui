LIB = ../../../../lib

TARGET = test_g0_lcd_ili9327
BUILD_DIR = build
CPU = -mcpu=cortex-m0plus
LDSCRIPT = stm32g030x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32g0x
OPT = -Os -g3

INC += .

SRC += $(TARGET).c

PERIPH = $(LIB)/hw/uc_hw/stm32g0x

INC += $(PERIPH)

SRC += $(PERIPH)/periph_rcc.c
SRC += $(PERIPH)/periph_gpio.c
SRC += $(PERIPH)/periph_spi.c

INC += $(PERIPH)/..
SRC += $(PERIPH)/../stm_pwm.c
SRC += $(PERIPH)/../systick.c

INC += $(LIB)/common

INC += $(LIB)/hw/drivers
INC += $(LIB)/hw/include

INC += $(LIB)/hw/api

SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ili9327.c


include $(LIB)/hw/system/uc_build.mk
