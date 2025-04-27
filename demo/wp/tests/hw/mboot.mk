

flash_boot: $(BUILD_DIR)/$(TARGET).infw
	fw_flash.sh $< 1 1 115200 n hw n

$(BUILD_DIR)/%.infw: $(BUILD_DIR)/%.bin
	~/wb/fw/libwbmcu-system/make_infw.sh $< $@

reset_wb:
	$(shell ssh $(wb_ssh) "./reboot_vbus_power.sh")
