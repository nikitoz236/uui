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


widget_metrics_list_test_run:

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
