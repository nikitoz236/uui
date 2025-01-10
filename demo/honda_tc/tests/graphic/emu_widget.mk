LIB = $(PROJECT)/../../lib

# emu_tc.h
# event_list.h
INC += $(PROJECT)/tests/graphic
SRC += $(PROJECT)/tests/graphic/emu_tc.c

# forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c
SRC += $(LIB)/graphic/forms/forms_align.c

# text_field.h
INC += $(LIB)/graphic/text

# systick.h
# rtc.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
