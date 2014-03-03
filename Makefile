CFLAGS = -Wall -Werror -Wextra -g
LIBS=-lSDL2 -lSDL2_image
CC = gcc

.PHONY: clean

all: anko_text anko

anko_text: src/text/display.o src/text/main.o src/board.o src/generator.o src/simulator.o
	$(CC) $(CFLAGS) -o $@ $^

anko: src/graphics/main.o src/graphics/render.o src/graphics/sprites.o src/graphics/context.o src/graphics/event.o src/generator.o src/simulator.o src/board.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f anko_text anko `find . -name "*.o" | xargs`
