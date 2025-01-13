PROJECT = ../../..
LIB = $(PROJECT)/../../lib

# tc_colors.h
# emu_tc.h
INC += ..
SRC += ../emu_tc.c

INC += .
SRC += widget_tc_dump.c



INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c

# dlc_poll.h
INC += ../../../logic/
SRC += ../emu_dlc_data.c
SRC += ../../../logic/honda_dlc_units.c

SRC += $(LIB)/graphic/forms/forms_split.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c

INC += $(LIB)/common/
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/val_mod.c

TEST_NAME = test_dump

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
