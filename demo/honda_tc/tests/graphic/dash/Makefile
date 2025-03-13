PROJECT = ../../..
LIB = $(PROJECT)/../../lib


# tc_colors.h
# emu_tc.h
INC += ..
SRC += ../emu_tc.c

INC += .
SRC += widget_dash.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c


INC += $(LIB)/common/
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/val_mod.c

dash_run:

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
