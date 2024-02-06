# позволяет скомпилировать и запустить консольные тесты
# нужно определить SRC, INC, DEF
# цель для запуска <имя файла с исходником теста>_test_run

%_test_app: %.c $(SRC)
	gcc -o $@ $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) $^

%_test_run: %_test_app
	./$<
	rm $<
