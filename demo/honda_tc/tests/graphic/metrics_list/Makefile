PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += $(PROJECT)/widgets
SRC += $(PROJECT)/widgets/widget_metric_list_item.c
SRC += $(PROJECT)/widgets/widget_metric_list.c

INC += ./$(LIB)/hw/include/

SRC += ./$(LIB)/emu/hw/systick.c

INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/trip_integrate.c


widget_metrics_list_test_run:

include $(LIB)/emu/test_graphic_color.mk
