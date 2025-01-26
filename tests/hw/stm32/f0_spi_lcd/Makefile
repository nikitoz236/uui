PROJECT = ..
LIB = ../../../../lib

TARGET = test_f0_spi_lcd
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
SRC += $(PERIPH)/../stm_pwm.c
SRC += $(PERIPH)/../systick.c

INC += $(LIB)/common

INC += $(LIB)/hw/drivers
INC += $(LIB)/hw/include

SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_pcf8814.c

include $(LIB)/hw/system/uc_build.mk
