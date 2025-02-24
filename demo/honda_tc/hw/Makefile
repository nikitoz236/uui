PROJECT = ..
LIB = $(PROJECT)/../../lib

TARGET = tc_uc_main
BUILD_DIR = build
CPU = -mcpu=cortex-m3
LDSCRIPT = stm32f103x8.ld
STACK_SIZE = 512
OPENOCD_TARGET ?= stm32f1x
OPT = -Os

DEF += STM32F10X_MD
SRC += $(TARGET).c
SRC += hw_cfg.c
SRC += kline.c
SRC += time_subsystem.c

INC += .
INC += $(LIB)/common
INC += $(LIB)/hw/drivers
INC += $(LIB)/hw/api/

INC += $(LIB)/hw/uc_hw
INC += $(LIB)/hw/uc_hw/stm32f1x

SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_gpio.c
SRC += $(LIB)/hw/uc_hw/stm32f1x/periph_spi.c

SRC += $(LIB)/hw/uc_hw/systick.c
SRC += $(LIB)/hw/uc_hw/stm_usart.c
SRC += $(LIB)/hw/uc_hw/stm_pwm.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_rtc.c

SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ili9341.c

SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c

SRC += $(LIB)/common/date_time.c


INC += ../logic
SRC += ../logic/dlc_poll.c
SRC += ../logic/honda_dlc_units.c
# SRC += ../logic/metrics_ecu.c
# SRC += ../logic/trip_integrate.c

INC += $(LIB)/storage
SRC += $(LIB)/storage/storage.c
SRC += $(LIB)/hw/uc_hw/stm32f10x_flash.c



# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

include $(LIB)/hw/system/uc_build.mk
