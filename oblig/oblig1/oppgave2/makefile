CC = gcc
CFLAGS = -std=gnu11 -g -Wall -Wextra

test: stringoperations.c tests.c
	gcc $(CFLAGS) stringoperations.c tests.c -o test

run: test
	./test

clean:
	rm -f test
