CFLAGS = -Wall -Werror -Wextra -g
CC = gcc

anko: display.o generator.o main.o simulator.o
	$(CC) $(CFLAGS) -o $@ $^
