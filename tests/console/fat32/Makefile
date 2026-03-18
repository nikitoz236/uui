LIB = ../../../lib

TEST_NAME = test_fat32

SRC += $(LIB)/emu/fat32/sd_cache.c

INC += $(LIB)/emu/fat32
INC += $(LIB)/hw/drivers
INC += $(LIB)/hw/include

SD_CARD_SIZE_MB ?= 32

include $(LIB)/emu/emu_dp_dep.mk
include $(LIB)/emu/test_console.mk

sd.img:
	dd if=/dev/zero of=sd.img bs=1M count=$(SD_CARD_SIZE_MB) && mkfs.fat -F32 sd.img

mount_sd:
	mkdir -p mnt && sudo mount -o loop,uid=$(shell id -u) sd.img mnt

umount_sd:
	sudo umount mnt
