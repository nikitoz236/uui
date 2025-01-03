
# ui_tree.h
INC += $(LIB)/graphic/ui_tree
SRC += $(LIB)/graphic/ui_tree/ui_tree.c

# debug_print.h
INC += $(LIB)/emu

# misc.h
INC += $(LIB)/common

# fonts.h
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/*.c

# lcd_text_color.h
INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/*.c

# forms_edge.h
INC += $(LIB)/graphic/forms


SRC += $(LIB)/graphic/lcd_text.c
SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/graphic/align_forms.c

include $(LIB)/emu/graphic/emu_graphic.mk
