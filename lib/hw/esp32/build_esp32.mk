



LD_FILES += $(LIB)/hw/esp32/system/esp32s3.ld
SRC += $(LIB)/hw/esp32/system/startup_esp32.c
TARGET = app

TOOLCHAIN_PATH = /home/nikita/.espressif/tools/xtensa-esp-elf/esp-15.2.0_20250929/xtensa-esp-elf/bin

# Путь к ESP32 S3 SDK (например, путь к ESP-IDF)
IDF_PATH = /home/nikita/dev/esp32/esp-idf

LD_FILES += $(IDF_PATH)/components/soc/esp32s3/ld/esp32s3.peripherals.ld

# esp_assert.h
INC += $(IDF_PATH)/components/esp_common/include

# esp32s3 soc
INC += $(IDF_PATH)/components/soc/include
INC += $(IDF_PATH)/components/soc/esp32s3/include
INC += $(IDF_PATH)/components/soc/esp32s3/register
TOOLCHAIN = $(TOOLCHAIN_PATH)/xtensa-esp32s3-elf

ESPTOOL_PATH = /home/nikita/.espressif/python_env/idf6.0_py3.12_env/bin

CC = $(TOOLCHAIN)-gcc

CFLAGS += -Wall
# CFLAGS += -Wextra
# CFLAGS += -Werror
# CFLAGS += -Wundef
# CFLAGS += -Wshadow
# CFLAGS += -Wdouble-promotion
# CFLAGS += -Wconversion
CFLAGS += -fno-common
# CFLAGS += -pedantic
CFLAGS += -mlongcalls
CFLAGS += -mtext-section-literals
CFLAGS += -nostdlib
# CFLAGS += -specs=nano.specs
CFLAGS += -finline-stringops # not need memcpy function

CFLAGS += -Os -ffunction-sections -fdata-sections

BUILD_DIR = build

all: $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(ESPTOOL_PATH)/esptool --chip esp32s3 elf2image --flash-mode dio --flash-freq 80m --flash-size 8MB --min-rev-full 0 --max-rev-full 99 $^
	$(ESPTOOL_PATH)/esptool image-info $@

$(BUILD_DIR)/$(TARGET).elf: $(SRC) $(BUILD_DIR) Makefile
	$(CC) $(CFLAGS) $(SRC) $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) $(addprefix -T, $(LD_FILES)) -o $@

asm: $(BUILD_DIR)/$(TARGET).elf
	$(TOOLCHAIN)-objdump -D $(BUILD_DIR)/$(TARGET).elf | less

PORT ?= /dev/ttyACM0

flash: $(BUILD_DIR)/$(TARGET).bin
	$(ESPTOOL_PATH)/esptool -p $(PORT) write-flash 0 $^

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

monitor:
	$(shell gnome-terminal -- python $(LIB)/../serial_monitor/sterm.py $(PORT) 115200)

reset:
	python $(LIB)/../serial_monitor/esp_reset.py $(PORT)

# include esp32_monitor.mk
