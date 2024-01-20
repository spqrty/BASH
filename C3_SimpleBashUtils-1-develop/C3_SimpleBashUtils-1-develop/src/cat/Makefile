CFLAGS = -std=c11 -Wall -Werror -Wextra


all: s21_cat

s21_cat: s21_cat.o
	gcc $(CFLAGS) flag.o cat.o -o s21_cat

s21_cat.o: lib_flag.o	
	gcc $(CFLAGS) -c cat.c -o cat.o	

lib_flag.o:
	gcc $(CFLAGS) -c ../common/flag.c -o flag.o

clean:
	rm -rf *.o s21_cat