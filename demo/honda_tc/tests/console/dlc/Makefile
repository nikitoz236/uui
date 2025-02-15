LIB = ../../../../../lib

PROJECT = ../../..

INC += ./
SRC += ./dlc_emu.c

INC += ./$(PROJECT)/logic/
SRC += ./$(PROJECT)/logic/dlc_poll.c

INC += ./$(LIB)/emu/console/
SRC += ./$(LIB)/emu/console/console_getc.c
SRC += ./$(LIB)/emu/console/emu_dp_usart.c

INC += ./$(LIB)/hw/include/
SRC += ./$(LIB)/emu/hw/systick.c

INC += ./$(LIB)/common/
SRC += ./$(LIB)/common/str_utils.c
SRC += ./$(LIB)/common/str_val.c
SRC += ./$(LIB)/common/str_val_buf.c

TEST_NAME = test_dlc

include $(LIB)/emu/test_console.mk
