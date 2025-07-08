LIB = ../../../../../lib

BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = gd32e23x_mboot.ld
STACK_SIZE = 512
OPT = -Os
DEF += STM32F030

TARGET = test_wp_lcd

PERIPH_HW = $(LIB)/hw/uc_hw/gd32e230

SRC += $(TARGET).c

INC += .
INC += $(LIB)/common/
INC += $(LIB)/hw/uc_hw/

INC += $(PERIPH_HW)
SRC += $(PERIPH_HW)/periph_rcc.c
SRC += $(PERIPH_HW)/periph_gpio.c
SRC += $(PERIPH_HW)/periph_spi.c

SRC += $(LIB)/hw/uc_hw/stm_pwm.c
SRC += $(LIB)/hw/uc_hw/systick.c

INC += $(LIB)/hw/api
INC += $(LIB)/hw/include

INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ST7789.c


SRC += $(LIB)/hw/uc_hw/stm_usart.c

INC += $(LIB)/common

SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c



SRC += ../mboot.c
include ../mboot.mk
include $(LIB)/hw/system/uc_build.mk
