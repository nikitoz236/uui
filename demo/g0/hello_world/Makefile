LIB = ../../../lib

TARGET = hello_world
BUILD_DIR = build
CPU = -mcpu=cortex-m0plus
LDSCRIPT = stm32g030x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32g0x
OPT = -Os -g3

INC += .

SRC += hw_cfg.c
SRC += main.c

# SRC += $(LIB)/hw/uc_hw/stm32f10x_rcc.c
# SRC += $(LIB)/hw/uc_hw/stm32f10x_gpio.c
# SRC += $(LIB)/hw/uc_hw/stm32f10x_usart.c
# SRC += $(LIB)/hw/uc_hw/stm32f10x_systick.c
# SRC += $(LIB)/hw/uc_hw/stm32f10x_pwm.c

INC += $(LIB)/common

# GCC_PATH = /home/nikita/ubuntu_home/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
