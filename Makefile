CFLAGS = -Wall -Werror -Wextra -g
CC = gcc

.PHONY: clean

anko: display.o generator.o main.o simulator.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f anko *.o
