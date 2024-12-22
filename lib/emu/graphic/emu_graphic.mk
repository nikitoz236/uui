# должен быть определен путь к LIB

# api_lcd_color.h
INC += $(LIB)/hw/api

# forms.h
# lcd_text.h
INC += $(LIB)/graphic

# emu_lcd.h
INC += $(LIB)/emu/graphic/

SRC += $(LIB)/emu/graphic/gfx.c
SRC += $(LIB)/emu/graphic/emu_lcd.c

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
