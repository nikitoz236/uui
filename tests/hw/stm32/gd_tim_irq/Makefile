PROJECT = ..
LIB = ../../../../lib

TARGET = test_gd_tim_irq
BUILD_DIR = build
CPU = -mcpu=cortex-m0
LDSCRIPT = gd32e23x_wbboot.ld
STACK_SIZE = 512
OPT = -Os

# DEF += STM32F103
DEF += STM32F030

INC += .
INC += $(LIB)/common/

SRC += $(LIB)/hw/uc_hw/stm_rcc_common.c
SRC += $(LIB)/hw/uc_hw/stm32f0x_rcc.c
SRC += $(LIB)/hw/uc_hw/stm32f0x_gpio.c

SRC += $(TARGET).c

# GCC_PATH = /home/nikita/software/gcc-arm-none-eabi-10-2020-q4-major/bin/

flash_boot: $(BUILD_DIR)/$(TARGET).infw
	fw_flash.sh $< 1 1 115200 n hw n

elf: $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.infw: $(BUILD_DIR)/%.bin
	~/wb/fw/libwbmcu-system/make_infw.sh $< $@

reset_wb:
	$(shell ssh wb_ridder "./reboot_vbus_power.sh" )

include $(LIB)/hw/system/uc_build.mk
