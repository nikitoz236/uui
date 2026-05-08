PROJECT = ..
LIB = ../../../../lib

TARGET = test_f0_spi_st7789_lcd_2_25__284_76
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
SRC += $(PERIPH)/../stm_pwm.c

INC += $(LIB)/common

INC += $(LIB)/hw/drivers
INC += $(LIB)/hw/include
INC += $(LIB)/hw/api

SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/bl_pwm.c
SRC += $(LIB)/hw/drivers/lcd_ST7789.c

include $(LIB)/hw/system/uc_build.mk
