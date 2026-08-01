LIB = ../../../../../lib

TARGET = test_bat_mon_lcd
BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = stm32f051x8.ld
STACK_SIZE = 512
OPT = -Os

OPENOCD_TARGET ?= stm32f0x

DEF += STM32F051
DEF += LCD_TOP_LSB

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
INC += $(LIB)/graphic
INC += $(LIB)/graphic/lcd_mono
INC += $(LIB)/graphic/text
INC += $(LIB)/graphic/fonts
INC += $(LIB)/graphic/forms

SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/bl_pwm.c
SRC += $(LIB)/hw/drivers/lcd/lcd_st7567.c
SRC += $(LIB)/graphic/lcd_mono/lcd_fb.c
SRC += $(LIB)/graphic/lcd_mono/print_char.c
SRC += $(LIB)/graphic/fonts/fonts.c
SRC += $(LIB)/graphic/fonts/font_5x5.c
SRC += $(LIB)/graphic/text/text_print.c
SRC += $(LIB)/graphic/text/text_pointer.c
SRC += $(LIB)/graphic/text/font_config.c

include $(LIB)/hw/system/uc_build.mk
