
#	LIB_SRC - содержит библиотечные исходники абстрактного кода микроконтроллера для устройства
#	EMU_SRC - содержит исходники среды исполнения эмуляции

INC += $(UCLIB_PATH)/emu
INC += $(UCLIB_PATH)/forms
INC += $(UCLIB_PATH)/widgets
INC += $(UCLIB_PATH)/api
INC += $(UCLIB_PATH)/fonts
INC += $(UCLIB_PATH)/common

EMU_SRC += $(UCLIB_PATH)/emu/*.c
LIB_SRC += $(UCLIB_PATH)/forms/*.c
LIB_SRC += $(UCLIB_PATH)/widgets/*.c
LIB_SRC += $(UCLIB_PATH)/fonts/*.c
# LIB_SRC += $(UCLIB_PATH)/common/*.c
GFX_FLAGS = -lX11 -lm -I /usr/X11R6/include

emu: emu_app
	./$<
	rm $<

emu_app: $(LIB_SRC) $(EMU_SRC) $(SRC)
	gcc $^ $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) $(GFX_FLAGS) -o $@
