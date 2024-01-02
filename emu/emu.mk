
#	LIB_SRC - содержит библиотечные исходники абстрактного кода микроконтроллера для устройства
#	EMU_SRC - содержит исходники среды исполнения эмуляции

INC += $(UCLIB_PATH)/emu
INC += $(UCLIB_PATH)/forms
EMU_SRC += $(UCLIB_PATH)/emu/*.c
LIB_SRC += $(UCLIB_PATH)/forms/*.c
GFX_FLAGS = -lX11 -lm -I /usr/X11R6/include

emu: emu_app
	./$<
	rm $<

emu_app: $(LIB_SRC) $(EMU_SRC) $(SRC)
	gcc $^ $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) $(GFX_FLAGS) -o $@
