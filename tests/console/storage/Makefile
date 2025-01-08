LIB = ../../../lib

all: storage_test_run

INC += ./
INC += $(LIB)/storage
INC += $(LIB)/hw/include
INC += $(LIB)/common

# image in ram
# SRC += ./emu_flash_in_ram.c

# image in file
INC += $(LIB)/emu/storage
SRC += $(LIB)/emu/storage/emu_storage.c

SRC += $(LIB)/storage/storage.c

include $(LIB)/emu/test_console.mk
