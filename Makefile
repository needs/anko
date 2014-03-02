CFLAGS = -Wall -Werror -Wextra -g
CC = gcc

anko: display.o main.o
	$(CC) $(CFLAGS) -o $@ $^
