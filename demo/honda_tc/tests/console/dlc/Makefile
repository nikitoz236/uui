LIB = ../../../../../lib

PROJECT = ../../..

all: dlc_test_run

INC += ./
SRC += ./dlc_emu.c

INC += ./$(PROJECT)/logic/
SRC += ./$(PROJECT)/logic/dlc_poll.c

INC += ./$(LIB)/emu/console/
SRC += ./$(LIB)/emu/console/console_getc.c

INC += ./$(LIB)/hw/include/
SRC += ./$(LIB)/emu/hw/systick.c

INC += ./$(LIB)/common/
SRC += ./$(LIB)/common/str_utils.c

include $(LIB)/emu/test_console.mk
