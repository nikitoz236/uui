
# должен быть определен путь к LIB

INC += $(LIB)/hw/api

INC += $(LIB)/common
SRC += $(LIB)/common/*.c

INC += $(LIB)/emu/
SRC += $(LIB)/emu/*.c

INC += $(LIB)/emu/widgets
SRC += $(LIB)/emu/widgets/*.c

INC += $(LIB)/graphic
SRC += $(LIB)/graphic/*.c

INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/*.c

INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/*.c

INC += $(LIB)/graphic/ui_tree
SRC += $(LIB)/graphic/ui_tree/*.c

INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/*.c

# INC += $(LIB)/graphic/widgets

GFX_FLAGS = -lX11 -lm -I /usr/X11R6/include

%_test_app: %.c $(SRC)
	gcc $^ $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) $(GFX_FLAGS) -o $@

%_test_run: %_test_app
	./$<
	rm $<

src_print:
	echo $(SRC)

inc_print:
	echo $(INC)

clean:
	rm -f *_test_app
