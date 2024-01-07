UCLIB_PATH = .

INC += $(UCLIB_PATH)/api
SRC += ./main.c

include $(UCLIB_PATH)/emu/emu.mk
