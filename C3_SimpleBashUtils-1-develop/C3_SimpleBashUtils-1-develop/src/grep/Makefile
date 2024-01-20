CFLAGS = -std=c11 -Wall -Werror -Wextra

all: s21_grep

s21_grep: s21_grep.o
	gcc $(CFLAGS) flag.o grep.o -o s21_grep

s21_grep.o: lib_flag.o
	gcc $(CFLAGS) -c ./grep.c -o grep.o

lib_flag.o:
	gcc $(CFLAGS) -c ../common/flag.c -o flag.o

clean:
	rm -rf *.o s21_grep
