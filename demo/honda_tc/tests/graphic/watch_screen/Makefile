PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += ..
SRC += ../emu_tc.c

INC += $(PROJECT)/view
INC += $(PROJECT)/widgets
# SRC += $(PROJECT)/widgets/widget_watch_screen.c
SRC += $(PROJECT)/widgets/widget_time_settings.c


INC += $(PROJECT)/widgets/common
# SRC += $(PROJECT)/widgets/common/widget_screen_switch.c
SRC += $(PROJECT)/widgets/common/widget_selectable_list.c
# SRC += $(PROJECT)/widgets/common/widget_text.c
SRC += $(PROJECT)/widgets/widget_settings_title.c

INC += $(PROJECT)/ui
# SRC += $(PROJECT)/ui/ui_mod.c

INC += $(LIB)/graphic/ui_tree/widgets
# SRC += $(LIB)/graphic/ui_tree/widgets/widget_test_rect_with_text.c

# forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c


INC += $(LIB)/common/
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/date_time.c
SRC += $(LIB)/common/val_mod.c

#depricated
SRC += $(LIB)/graphic/align_forms.c

TEST_NAME = watch_screen

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
