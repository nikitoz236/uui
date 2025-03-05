
# ui_tree.h
INC += $(LIB)/graphic/ui_tree
SRC += $(LIB)/graphic/ui_tree/ui_tree.c

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
SRC += $(LIB)/graphic/forms/forms_split.c

# text_field.h
INC += $(LIB)/graphic/text
SRC += $(LIB)/graphic/text/text_label_color.c
SRC += $(LIB)/graphic/text/text_field.c

# for text_label_color
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/val_text.c

SRC += $(LIB)/graphic/lcd_text.c
SRC += $(LIB)/common/str_utils.c

include $(LIB)/emu/graphic/emu_graphic.mk
