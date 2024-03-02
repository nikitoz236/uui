LIB = ../../../../../lib

all: metrics_view_test_run

INC += ../../../logic
INC += $(LIB)/common
INC += $(LIB)/hw/include

SRC += ../../../logic/metrics_view.c
SRC += ../../../logic/trip_integrate.c
SRC += $(LIB)/emu/hw/systick.c

include $(LIB)/emu/test_console.mk
