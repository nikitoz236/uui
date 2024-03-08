LIB = ../../lib

all: storage_test_test_run

INC += ./
INC += $(LIB)/storage
INC += $(LIB)/hw/include
INC += $(LIB)/common

SRC += ./emu_flash.c
SRC += $(LIB)/storage/storage.c

include $(LIB)/emu/test_console.mk
