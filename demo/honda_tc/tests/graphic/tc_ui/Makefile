PROJECT = ../../..
LIB = $(PROJECT)/../../lib

# emu
INC += ..
SRC += ../emu_tc.c
SRC += ../emu_dlc_data.c
# dp dependencies
SRC += $(LIB)/emu/console/emu_dp_usart.c


# INC += $(PROJECT)/view
# SRC += $(PROJECT)/view/view.c


INC += $(PROJECT)/widgets

INC += $(PROJECT)/widgets/common/
# SRC += $(PROJECT)/widgets/widget_debug_metrics.c

SRC += $(PROJECT)/widgets/common/widget_metric_list.c
SRC += $(PROJECT)/widgets/widget_metric_list_item.c

SRC += $(PROJECT)/widgets/widget_debug_metrics.c

SRC += $(PROJECT)/widgets/common/widget_screen_switch.c
# SRC += $(PROJECT)/widgets/common/widget_titled_screen.c

INC += $(LIB)/graphic/ui_tree/widgets
# SRC += $(LIB)/graphic/ui_tree/widgets/widget_test_rect_with_text.c


SRC += $(PROJECT)/widgets/common/widget_selectable_list.c
#depricated
SRC += $(LIB)/graphic/align_forms.c


INC += ../dump
SRC += ../dump/widget_tc_dump.c


SRC += $(LIB)/graphic/forms/forms_split.c



# dlc_poll.h
INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/trip_integrate.c
SRC += $(PROJECT)/logic/honda_dlc_units.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c

# common
INC += $(LIB)/common/
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/val_mod.c

TEST_NAME = tc_ui

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
