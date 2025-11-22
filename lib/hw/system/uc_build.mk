
INC += $(LIB)/hw/system/include
INC += $(LIB)/hw/system/cmsis
INC += $(LIB)/hw/include
# INC += $(LIB)/hw/uc_hw

SRC += $(LIB)/hw/system/startup_cm.c

LD_DIR = $(LIB)/hw/system/ldscripts
LDSCRIPT += sections.ld
LD_FILES += $(BUILD_DIR)/stack_size.ld
LD_FILES += $(addprefix $(LD_DIR)/, $(LDSCRIPT))

PREFIX = arm-none-eabi-
CC = $(GCC_PATH)$(PREFIX)gcc
CP = $(GCC_PATH)$(PREFIX)objcopy
SZ = $(GCC_PATH)$(PREFIX)size
OD = $(GCC_PATH)$(PREFIX)objdump
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

MCU = $(CPU) -mthumb

OOCD ?= openocd
OOCD_INTERFACE ?= stlink
FLASH_TARGET ?= $(BUILD_DIR)/$(TARGET).bin
OOCD_INTERFACE_FILE ?= interface/$(OOCD_INTERFACE).cfg

include $(wildcard $(BUILD_DIR)/*.d)

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf $(BUILD_DIR)
	$(BIN) $< $@

OPT += -lgcc
OPT += -Wall
# OPT += -fno-builtin
OPT += -nostdlib
# OPT += -mthumb
# OPT += -Wl,--gc-sections
# OPT += -specs=nano.specs
OPT += -Wl,-Map,"$(BUILD_DIR)/map_file"
OPT += -g3
# OPT += -MMD

$(BUILD_DIR)/$(TARGET).elf: $(SRC) $(BUILD_DIR)/stack_size.ld Makefile
	$(CC) $(SRC) $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) $(addprefix -T, $(LD_FILES)) $(OPT) $(MCU) -o $@
	$(SZ) $@

$(BUILD_DIR)/stack_size.ld: $(BUILD_DIR)
	$(shell echo "_Minimum_Stack_Size = $(STACK_SIZE);" > $@)

$(BUILD_DIR):
	mkdir $@

size: $(BUILD_DIR)/$(TARGET).elf
	$(SZ) $<

clean:
	-rm -rf $(BUILD_DIR)

asm: $(BUILD_DIR)/$(TARGET).elf
	$(OD) -D $< | less

hex_dump: $(BUILD_DIR)/$(TARGET).bin
	xxd -g1 $< | less

flash_reset: $(FLASH_TARGET)
	$(OOCD) -f $(OOCD_INTERFACE_FILE) \
	-f target/$(OPENOCD_TARGET).cfg \
	-c "reset_config srst_only srst_nogate connect_assert_srst" \
	-c "program $(FLASH_TARGET) verify 0x8000000; reset run; exit"

flash: $(FLASH_TARGET)
	$(OOCD) -f $(OOCD_INTERFACE_FILE) \
	-f target/$(OPENOCD_TARGET).cfg \
	-c "program $(FLASH_TARGET) verify 0x8000000; reset run; exit"

dump:
	$(OOCD) -f $(OOCD_INTERFACE_FILE) \
	-f target/$(OPENOCD_TARGET).cfg \
	-c "init" \
	-c "reset halt" \
	-c "flash banks" \
	-c "dump_image $(shell date +"flash_dump_%y%m%d_%H%M%S.bin") 0x08000000 0x10000" \
	-c "poll" \
	-c "reset run" \
	-c "exit"

erase:
	@echo ERASE
	$(OOCD) -f $(OOCD_INTERFACE_FILE) \
	-f target/$(OPENOCD_TARGET).cfg \
	-c "init" \
	-c "reset halt" \
	-c "stm32f1x mass_erase 0" \
	-c "exit"

reset:
	@echo RESET
	$(OOCD) -f $(OOCD_INTERFACE_FILE) \
	-f target/$(OPENOCD_TARGET).cfg \
	-c "init" \
	-c "reset run" \
	-c "exit"

swd_dbg: $(BUILD_DIR)/$(TARGET).elf
	$(shell gnome-terminal -- arm-none-eabi-gdb $< -ex "target remote localhost:3333")
	$(OOCD) -f $(OOCD_INTERFACE_FILE) \
	-f target/$(OPENOCD_TARGET).cfg

minicom:
	$(shell gnome-terminal -- minicom -D /dev/ttyACM0 -b 115200)
