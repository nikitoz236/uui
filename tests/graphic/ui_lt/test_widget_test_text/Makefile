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

# draw_color.h, lcd_text_color.h, lcd_color_tptr_print
INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/lcd_text_color.c

# text_field.h, text_pointer.h, font_config.h
INC += $(LIB)/graphic/text
SRC += $(LIB)/graphic/text/text_pointer.c
SRC += $(LIB)/graphic/text/font_config.c

# fonts
SRC += $(LIB)/graphic/fonts/font_5x7.c
SRC += $(LIB)/graphic/fonts/fonts.c
INC += $(LIB)/graphic/fonts

# forms_split.h (нужен draw_frame)
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/forms_split.c

TEST_NAME = test_widget_test_text

include $(LIB)/emu/emu_dp_dep.mk
include $(LIB)/emu/graphic/emu_graphic.mk
