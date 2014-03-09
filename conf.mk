#
# Configuration de la compilation
#
CFLAGS = -Wall -Werror -Wextra -g -Isrc
BINDIR = .
CC = gcc

#
# Binaires à construire
#
BINS = anko anko_text

SRCS_anko_text = src/game/*.c src/text/*.c
LDFLAGS_anko_text =
CFLAGS_anko_text = $(CFLAGS)

SRCS_anko = src/game/*.c src/client/*.c src/client/ui/*.c
LDFLAGS_anko = -lglfw -lGL -lm -lGLEW
CFLAGS_anko = $(CFLAGS)

# We create textures_pack.def if missing to avoid include error.
$(shell test -f src/client/textures_pack.def || touch src/client/textures_pack.def)

src/client/textures.def: src/client/textures_pack.def

src/client/textures_pack.def: data/tiles.def data/entities.def data/particles.def
	@cat $^ > $@

data/tiles.def: pack.sh $(wildcard data/tiles/*.png)
	@./pack.sh data/tiles.png $(wordlist 2, $(words $^), $^) > $@

data/entities.def: pack.sh $(wildcard data/entities/*.png)
	@./pack.sh data/entities.png $(wordlist 2, $(words $^), $^) > $@

data/particles.def: pack.sh $(wildcard data/particles/*.png)
	@./pack.sh data/particles.png $(wordlist 2, $(words $^), $^) > $@

custom_clean:
	@rm -f data/tiles.{png,def} data/entities.{png,def} data/particles.{png,def} src/client/textures_pack.def
