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

tc_view_test_run:

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
