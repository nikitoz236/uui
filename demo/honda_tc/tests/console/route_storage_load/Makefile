PROJECT = ../../..
LIB = $(PROJECT)/../../lib

# routes.h
INC += ./$(PROJECT)/logic/
SRC += ./$(PROJECT)/logic/routes.c

# storage
INC += ./$(LIB)/storage/
SRC += ./$(LIB)/storage/storage.c
INC += ./$(LIB)/emu/storage/
SRC += ./$(LIB)/emu/storage/emu_storage.c

# storage_hw.h
INC += ./$(LIB)/../tests/console/storage/

# systick.h
INC += $(LIB)/hw/include/
# SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c

# date_time.h
INC += $(LIB)/common/
# SRC += $(LIB)/common/date_time.c

# dp dependencys
SRC += $(LIB)/emu/console/emu_dp_usart.c
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c
# SRC += $(LIB)/common/val_mod.c

TEST_NAME = route_storage_load

include $(LIB)/emu/test_console.mk
