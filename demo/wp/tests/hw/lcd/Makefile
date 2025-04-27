LIB = ../../../../../lib

BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = gd32e23x_mboot.ld
STACK_SIZE = 512
OPT = -Os
DEF += STM32F030

TARGET = test_wp_lcd

INC += .
INC += $(LIB)/common/
INC += $(LIB)/hw/uc_hw/
INC += $(LIB)/hw/uc_hw/stm32f0x

SRC += $(TARGET).c

SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f0x/periph_spi.c

SRC += $(LIB)/hw/uc_hw/stm_pwm.c
SRC += $(LIB)/hw/uc_hw/systick.c

INC += $(LIB)/hw/api
INC += $(LIB)/hw/include

INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ST7789.c


SRC += ../mboot.c
include ../mboot.mk
include $(LIB)/hw/system/uc_build.mk
