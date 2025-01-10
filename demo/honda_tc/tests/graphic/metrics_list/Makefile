PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += $(PROJECT)/widgets
INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/widget_metric_list_item.c
SRC += $(PROJECT)/widgets/common/widget_metric_list.c

INC += $(PROJECT)/view

INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/trip_integrate.c

INC += ./$(LIB)/hw/include/
SRC += ./$(LIB)/emu/hw/systick.c

INC += ..

#depricated
SRC += $(LIB)/graphic/align_forms.c

TEST_NAME = test_widget_metric_list

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
