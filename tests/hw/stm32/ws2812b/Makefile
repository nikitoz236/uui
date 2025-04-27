PROJECT = ..
LIB = ../../../../lib

TARGET = test_ws2812b
BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = gd32e23x_mboot.ld
STACK_SIZE = 512
OPT = -Os

DEF += STM32F030

INC += .
INC += $(LIB)/common/

PERIPH = $(LIB)/hw/uc_hw/stm32f0x

INC += $(PERIPH)
INC += $(PERIPH)/../

SRC += $(PERIPH)/periph_rcc.c
SRC += $(PERIPH)/periph_gpio.c

# SRC += $(LIB)/hw/uc_hw/stm32f10x_pwm.c

SRC += test_ws2812b.c
SRC += hw_cfg.c

# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/



flash_boot: $(BUILD_DIR)/$(TARGET).infw
	fw_flash.sh $< 1 1 115200 n hw n

$(BUILD_DIR)/%.infw: $(BUILD_DIR)/%.bin
	~/wb/fw/libwbmcu-system/make_infw.sh $< $@

reset_wb:
	$(shell ssh wb_ridder "./reboot_vbus_power.sh" )

include $(LIB)/hw/system/uc_build.mk
