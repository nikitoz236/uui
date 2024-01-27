INC += ./
INC += ../../

SRC += ./*.c
SRC += ../../*.c

test: date_time_test
	./$^
	rm $^

%_test: %_test.c $(SRC)
	gcc $^ $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) -o $@
