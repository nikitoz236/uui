PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += ..
SRC += ../emu_tc.c

INC += $(PROJECT)/view
INC += $(PROJECT)/widgets
SRC += $(PROJECT)/widgets/widget_watch_screen.c
SRC += $(PROJECT)/widgets/widget_time_settings.c


INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c

INC += $(LIB)/graphic/ui_tree/widgets
SRC += $(LIB)/graphic/ui_tree/widgets/widget_test_rect_with_text.c

# forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c


INC += $(LIB)/common/
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/date_time.c

watch_screen_test_run:

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
