# позволяет скомпилировать и запустить консольные тесты
# нужно определить SRC, INC, DEF
# цель для запуска <имя файла с исходником теста>_run

%_app: %.c $(SRC)
	gcc -o $@ $(CFLAGS) $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) -m32 $^

%_run: %_app
	./$<
	rm $<

src_print:
	echo $(SRC)

inc_print:
	echo $(INC)
