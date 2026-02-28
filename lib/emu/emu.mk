# позволяет скомпилировать и запустить консольные тесты
# нужно определить SRC, INC, DEF
# цель для запуска TEST_NAME = <имя файла с исходником теста>

$(TEST_NAME)_run:

build_check: $(TEST_NAME)_app
	rm ./$(TEST_NAME)_app

%_run: %_app
	./$<
	rm $<

clean:
	rm -f *_app
	rm -rf $(AI_DIR)

src_print:
	$(shell for s in $(SRC) ; do ls $$s ; done)

inc_print:
	echo $(INC)
	$(shell for s in $(INC) ; do echo $(s) ; done)
