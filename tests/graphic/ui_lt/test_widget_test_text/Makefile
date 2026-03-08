LIB = ../../../../lib
WIDGETS = ../widgets

# ui_lt.h, ltree.h
INC += $(LIB)/graphic/ui_tree
SRC += $(LIB)/graphic/ui_tree/ui_lt.c
INC += $(LIB)/common
SRC += $(LIB)/common/ltree.c

# widget__test_text.h
INC += $(WIDGETS)
SRC += $(WIDGETS)/widget__test_text.c

# draw_color.h
INC += $(LIB)/graphic/lcd_color


# forms_split.h (нужен draw_frame)
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

TEST_NAME = test_widget_test_text

include $(LIB)/emu/emu_dp_dep.mk
include $(LIB)/emu/graphic/emu_graphic.mk
