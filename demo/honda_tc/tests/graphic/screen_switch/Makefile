PROJECT = ../../..
LIB = $(PROJECT)/../../lib

# INC += $(PROJECT)/widgets
# SRC += $(PROJECT)/widgets/*.c

INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c

INC += ./$(LIB)/graphic/ui_tree/widgets
SRC += ./$(LIB)/graphic/ui_tree/widgets/widget_test_rect_with_text.c

INC += $(PROJECT)/view

# INC += ./$(LIB)/hw/include/

INC += ./
SRC += ./*.c

# SRC += ./$(LIB)/emu/hw/systick.c

# INC += $(PROJECT)/logic
# SRC += $(PROJECT)/logic/metrics_view.c
# SRC += $(PROJECT)/logic/metrics_ecu.c
# SRC += $(PROJECT)/logic/trip_integrate.c


screen_switch_test_run:

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
