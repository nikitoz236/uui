LIB = ../../../../../lib

TEST_NAME = storage_problem

INC += ./
INC += $(LIB)/storage
INC += $(LIB)/hw/include
INC += $(LIB)/common


SRC += $(LIB)/emu/console/emu_dp_usart.c
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c


SRC += $(LIB)/storage/storage.c

# image in ram
# SRC += ./emu_flash_in_ram.c

# image in file
INC += $(LIB)/emu/storage
SRC += $(LIB)/emu/storage/emu_storage.c

include $(LIB)/emu/test_console.mk
