#
# Configuration de la compilation
#
CFLAGS = -Wall -Wextra -Werror -g
BINDIR = .


#
# Binaires à construire
#
BINS = anko anko_text

SRCS_anko_text = src/*.c src/text/*.c
LDFLAGS_anko_text =
CFLAGS_anko_text = $(CFLAGS)

SRCS_anko = src/*.c src/client/*.c
LDFLAGS_anko = -lglfw -lGL
CFLAGS_anko = $(CFLAGS)
