LIB = ../../../../../lib

PROJECT = ../../..

all: tc_test_test_run

INC += ../dlc/
SRC += ../dlc/dlc_emu.c

INC += ./$(PROJECT)/logic/
SRC += ./$(PROJECT)/logic/dlc_poll.c
SRC += ./$(PROJECT)/logic/trip_integrate.c
SRC += ./$(PROJECT)/logic/metrics_ecu.c
SRC += ./$(PROJECT)/logic/routes.c
SRC += ./$(PROJECT)/logic/tc.c

INC += ./$(PROJECT)/view/

INC += ./$(LIB)/../tests/storage/

INC += ./$(LIB)/emu/storage/
SRC += ./$(LIB)/emu/storage/emu_storage.c

INC += ./$(LIB)/storage/
SRC += ./$(LIB)/storage/storage.c

INC += ./$(LIB)/emu/console/
SRC += ./$(LIB)/emu/console/console_getc.c

INC += ./$(LIB)/hw/include/
SRC += ./$(LIB)/emu/hw/systick.c
SRC += ./$(LIB)/emu/hw/rtc.c

INC += ./$(LIB)/common/
SRC += ./$(LIB)/common/str_utils.c

include $(LIB)/emu/test_console.mk
