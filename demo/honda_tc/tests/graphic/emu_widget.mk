LIB = $(PROJECT)/../../lib

# emu_tc.h
INC += $(PROJECT)/tests/graphic
SRC += $(PROJECT)/tests/graphic/emu_tc.c

# tc_events.h
INC += $(PROJECT)/view

# forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
