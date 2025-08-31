CC = gcc
FLAGS = -std=c11 -Wall -Werror -Wextra -pedantic

OBJ = $(FUNC:.c=.o)
FUNC = $(wildcard *.c)

all: decimal.a

decimal.a: $(OBJ)
	ar rcs decimal.a $^ 

clean:
	rm -f *.o *.a *.info
	rm -f ./tests/TEST*
	rm -rf ./tests/coverage_report/
.c.o:
	$(CC) $(FLAGS)  -c $< -o $@	

rebuild: clean all

clang:
	mv ../materials/linters/.clang-format .
	clang-format -i *.c *.h tests/*.c
	mv .clang-format ../materials/linters 

test: 
	$(CC) $(FLAGS) -lm -lcheck *.c ./tests/decimal_test.c -o ./tests/TEST
	./tests/TEST

gcov_report: 
	$(CC) --coverage $(FLAGS) -lm  -lcheck *.c ./tests/decimal_test.c -o ./tests/TEST
	./tests/TEST
	lcov -t "teplishko" -o teplishko.info -c -d ./tests
	genhtml -o ./tests/coverage_report ./teplishko.info
	
