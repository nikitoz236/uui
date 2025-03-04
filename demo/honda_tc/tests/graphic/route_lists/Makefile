PROJECT = ../../..
LIB = $(PROJECT)/../../lib

# tc_colors.h
# emu_tc.h
INC += ..
SRC += ../emu_tc.c

INC += .
SRC += widget_route_list_item.c



INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/common/widget_selectable_list.c

# storage
INC += ./$(LIB)/../tests/console/storage/
INC += ./$(LIB)/storage/
SRC += ./$(LIB)/storage/storage.c
INC += ./$(LIB)/emu/storage/
SRC += ./$(LIB)/emu/storage/emu_storage.c

# dlc_poll.h
SRC += ../emu_dlc_data.c
INC += $(PROJECT)/logic/
SRC += $(PROJECT)/logic/honda_dlc_units.c
SRC += $(PROJECT)/logic/routes.c
SRC += $(PROJECT)/logic/trip_integrate.c

# graphic
SRC += $(LIB)/graphic/forms/forms_split.c

# systick.h
INC += $(LIB)/hw/include/
SRC += $(LIB)/emu/hw/systick.c
SRC += $(LIB)/emu/hw/rtc.c

# dp dependencies
SRC += $(LIB)/emu/console/emu_dp_usart.c
SRC += $(LIB)/common/str_utils.c

INC += $(LIB)/common/
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c
SRC += $(LIB)/common/val_mod.c
SRC += $(LIB)/common/date_time.c

TEST_NAME = test_widget_route_list

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
