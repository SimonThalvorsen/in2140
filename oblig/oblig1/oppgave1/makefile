CC = gcc
CFLAGS = -std=gnu11 -g -Wall -Wextra

vowelshift:vowelshift.c
	gcc $(CFLAGS) vowelshift.c -o vowelshift

run: vowelshift
	./vowelshift "Lorem ipsum dolor sit amet" "X"

clean:
	rm -f vowelshift 
