#
# Configuration de la compilation
#
CFLAGS = -Wall -Werror -Wextra -g
BINDIR = .
CC = gcc


#
# Binaires à construire
#
BINS = anko anko_text

SRCS_anko_text = src/*.c src/text/*.c
LDFLAGS_anko_text =
CFLAGS_anko_text = $(CFLAGS)

SRCS_anko = src/*.c src/client/*.c
LDFLAGS_anko = -lglfw -lGL -lm -lGLEW
CFLAGS_anko = $(CFLAGS)

# We create textures_pack.def if missing to avoid include error.
$(shell test -f src/client/textures_pack.def || touch src/client/textures_pack.def)

src/client/textures.def: src/client/textures_pack.def

src/client/textures_pack.def: data/tiles.png data/entities.png

data/tiles.png: pack.sh $(wildcard data/tiles/*.png)
	@./pack.sh $@ $(wordlist 2, $(words $^), $^) >> src/client/textures_pack.def

data/entities.png: pack.sh $(wildcard data/entities/*.png)
	@./pack.sh $@ $(wordlist 2, $(words $^), $^) >> src/client/textures_pack.def

custom_clean:
	@rm -f data/tiles.png data/entities.png src/client/textures_pack.def
