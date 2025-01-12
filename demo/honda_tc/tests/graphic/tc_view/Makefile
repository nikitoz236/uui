PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += ./$(LIB)/graphic/ui_tree/widgets
SRC += ./$(LIB)/graphic/ui_tree/widgets/widget_test_rect_with_text.c

INC += $(PROJECT)/view
SRC += $(PROJECT)/view/view.c

INC += $(PROJECT)/widgets
SRC += $(PROJECT)/widgets/widget_metric_list_item.c
SRC += $(PROJECT)/widgets/widget_debug_metrics.c

INC += $(PROJECT)/widgets/common/
SRC += $(PROJECT)/widgets/common/widget_metric_list.c
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c
SRC += $(PROJECT)/widgets/common/widget_titled_screen.c

INC += ./$(LIB)/hw/include/

SRC += ./$(LIB)/emu/hw/systick.c

INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/trip_integrate.c

INC += ..
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


#depricated
SRC += $(LIB)/graphic/align_forms.c

TEST_NAME = tc_view

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
