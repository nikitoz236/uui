PROJECT = ../../..
LIB = $(PROJECT)/../../lib

# emu
INC += ..
SRC += ../emu_tc.c
# SRC += ../emu_dlc_data.c
INC += ../../console/dlc
SRC += ../../console/dlc/dlc_emu.c
# dp dependencies
SRC += $(LIB)/emu/console/emu_dp_usart.c
SRC += $(LIB)/emu/console/console_getc.c


# INC += $(PROJECT)/view
# SRC += $(PROJECT)/view/view.c


INC += $(PROJECT)/widgets
INC += $(PROJECT)/widgets/common/
INC += $(LIB)/graphic/ui_tree/widgets

SRC += $(PROJECT)/widgets/common/widget_selectable_list.c
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c
SRC += $(PROJECT)/widgets/common/widget_screen_by_idx.c

SRC += $(PROJECT)/widgets/widget_menu_screen.c

SRC += $(PROJECT)/widgets/widget_metric_list_item.c
SRC += $(PROJECT)/widgets/widget_route_list_item.c
SRC += $(PROJECT)/widgets/widget_dlc_dump.c



INC += ../route_lists

INC += ../dump


SRC += $(LIB)/graphic/forms/forms_split.c


# graphic
SRC += $(LIB)/graphic/text/text_field.c

# storage
INC += ./$(LIB)/../tests/console/storage/
INC += ./$(LIB)/storage/
SRC += ./$(LIB)/storage/storage.c
INC += ./$(LIB)/emu/storage/
SRC += ./$(LIB)/emu/storage/emu_storage.c

# dlc_poll.h
INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/dlc_poll.c
SRC += $(PROJECT)/logic/routes.c
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
SRC += $(LIB)/common/date_time.c


TEST_NAME = tc_ui

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
