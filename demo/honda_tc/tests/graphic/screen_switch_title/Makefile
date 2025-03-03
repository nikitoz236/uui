PROJECT = ../../..
LIB = $(PROJECT)/../../lib

# INC += $(PROJECT)/widgets
# SRC += $(PROJECT)/widgets/*.c

INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c

# INC += $(PROJECT)/view

INC += ./
SRC += ./*.c


# emu_tc.h
INC += ..
SRC += ../emu_tc.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c

# dp dependencies
SRC += $(LIB)/emu/console/emu_dp_usart.c
SRC += $(LIB)/common/str_utils.c

SRC += $(LIB)/common/val_mod.c

SRC += $(LIB)/graphic/forms/forms_split.c

TEST_NAME = test_screen_switch_title

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
