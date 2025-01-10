# позволяет скомпилировать и запустить консольные тесты
# нужно определить SRC, INC, DEF
# цель для запуска TEST_NAME = <имя файла с исходником теста>

include $(LIB)/emu/emu.mk

%_app: %.c $(SRC)
	gcc -o $@ $(CFLAGS) $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) -m32 $^
