PROJECT = ..
LIB = $(PROJECT)/../../lib

TARGET = tc_uc_main
BUILD_DIR = build
CPU = -mcpu=cortex-m3
LDSCRIPT = stm32f103x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32f1x
OPT = -Os

# DEF += STM32F103
DEF += STM32F10X_MD

INC += .
INC += $(LIB)/common
INC += $(LIB)/hw/drivers
INC += $(LIB)/hw/api/

SRC += tc_uc_main.c
SRC += hw_cfg.c

SRC += $(LIB)/hw/uc_hw/stm32f10x_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_usart.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_rtc.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_systick.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_pwm.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_spi.c
SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ili9341.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_utils.c

GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
