PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += ..
SRC += ../emu_tc.c

INC += $(PROJECT)/view
INC += $(PROJECT)/widgets
SRC += $(PROJECT)/widgets/widget_watch_screen.c

# forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

# systick.h
INC += ./$(LIB)/hw/include/
SRC += ./$(LIB)/emu/hw/systick.c
SRC += ./$(LIB)/emu/hw/rtc.c


INC += ./$(LIB)/common/
SRC += ./$(LIB)/common/str_val.c
SRC += ./$(LIB)/common/date_time.c

watch_screen_test_run:

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
