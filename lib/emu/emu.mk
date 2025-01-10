# позволяет скомпилировать и запустить консольные тесты
# нужно определить SRC, INC, DEF
# цель для запуска TEST_NAME = <имя файла с исходником теста>

$(TEST_NAME)_run:

build_check: $(TEST_NAME)_app
	rm ./$(TEST_NAME)_app

%_run: %_app
	./$<
	rm $<

src_print:
	echo $(SRC)

inc_print:
	echo $(INC)
