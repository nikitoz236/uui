# позволяет скомпилировать и запустить консольные тесты
# нужно определить SRC, INC, DEF
# цель для запуска <имя файла с исходником теста>_test_run

%_test_app: %.c $(SRC)
	gcc -o $@ $(CFLAGS) $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) -m32 $^

%_test_run: %_test_app
	./$<
	rm $<

src_print:
	echo $(SRC)

inc_print:
	echo $(INC)
