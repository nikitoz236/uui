PROJECT = ../../..
LIB = $(PROJECT)/../../lib

INC += $(PROJECT)/widgets/common
SRC += $(PROJECT)/widgets/common/widget_screen_switch.c

INC += ./$(LIB)/graphic/ui_tree/widgets
SRC += ./$(LIB)/graphic/ui_tree/widgets/widget_test_rect_with_text.c

INC += ./$(LIB)/hw/include/


INC += ./$(LIB)/graphic
SRC += ./$(LIB)/graphic/align_forms.c

INC += ..
INC += ./
SRC += ./*.c

SRC += ./$(LIB)/emu/hw/systick.c

INC += $(PROJECT)/view

INC += $(PROJECT)/logic
SRC += $(PROJECT)/logic/metrics_view.c
SRC += $(PROJECT)/logic/metrics_ecu.c
SRC += $(PROJECT)/logic/trip_integrate.c

TEST_NAME = selector

include $(LIB)/emu/graphic/emu_graphic_ui_tree.mk
