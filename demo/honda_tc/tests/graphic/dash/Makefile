PROJECT = ../../..
LIB = $(PROJECT)/../../lib


# tc_colors.h
# emu_tc.h
INC += ..
SRC += ../emu_tc.c

INC += .
SRC += widget_dash.c
SRC += node_widget_dash_watch.c

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

# emu
INC += ..
SRC += ../emu_tc.c
INC += ../../console/dlc
SRC += ../../console/dlc/dlc_emu.c

# logic
INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/dlc_poll.c
SRC += $(PROJECT)/logic/honda_dlc_units.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/routes.c
SRC += $(PROJECT)/logic/trip_integrate.c


# storage
INC += ./$(LIB)/../tests/console/storage/
INC += ./$(LIB)/storage/
SRC += ./$(LIB)/storage/storage.c
INC += ./$(LIB)/emu/storage/
SRC += ./$(LIB)/emu/storage/emu_storage.c


dash_run:

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
