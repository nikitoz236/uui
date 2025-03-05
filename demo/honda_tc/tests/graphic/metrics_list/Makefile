PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += $(PROJECT)/widgets
INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/common/widget_selectable_list.c
SRC += $(PROJECT)/widgets/widget_metric_list_item.c
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c

# SRC += $(PROJECT)/widgets/common/widget_metric_list.c

INC += $(PROJECT)/view

INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/trip_integrate.c

INC += ./$(LIB)/hw/include/
SRC += ./$(LIB)/emu/hw/systick.c

INC += ..

INC += $(LIB)/common/
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/val_mod.c
SRC += $(LIB)/common/str_utils.c

SRC += $(LIB)/emu/console/emu_dp_usart.c

TEST_NAME = test_widget_metric_list

include ../emu_widget.mk
