PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += ./$(LIB)/../tests/ui_layout
SRC += ./$(LIB)/../tests/ui_layout/widget_test_rect_with_text.c

INC += $(PROJECT)/view
SRC += $(PROJECT)/view/view.c

INC += $(PROJECT)/widgets
SRC += $(PROJECT)/widgets/widget_metric_list_item.c
SRC += $(PROJECT)/widgets/widget_metric_list.c

INC += $(PROJECT)/widgets/common/
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c
SRC += $(PROJECT)/widgets/common/widget_titled_screen.c

INC += ./$(LIB)/hw/include/

SRC += ./$(LIB)/emu/hw/systick.c

INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/trip_integrate.c

titled_screen_test_run:

include $(LIB)/emu/test_graphic_color.mk
