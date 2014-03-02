CFLAGS = -Wall -Werror -Wextra -g
LIBS=-lSDL2
CC = gcc

.PHONY: clean

all: anko_text anko

anko_text: src/text/display.o src/text/main.o src/generator.o src/simulator.o
	$(CC) $(CFLAGS) -o $@ $^

anko: src/graphics/main.o src/generator.o src/simulator.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f anko_text anko *.o src/*.o src/text/*.o src/graphics/*.o
