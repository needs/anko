CFLAGS = -Wall -Werror -Wextra -g
CC = gcc

.PHONY: clean

anko_text: src/text/display.o src/generator.o src/text/main.o src/simulator.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f anko_text *.o src/*.o src/text/*.o
