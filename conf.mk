#
# Flags
#
CFLAGS = -Wall -Werror -Wextra -pedantic -O0 -g -Isrc
BINDIR = .
CC = gcc

#
# Binaries
#
BINS = anko anko_server

SRCS_anko = src/client/main.c src/shared/*.c src/game/*.c
LDFLAGS_anko =
CFLAGS_anko = $(CFLAGS)

SRCS_anko_server = src/server/main.c src/shared/*.c src/game/*.c
LDFLAGS_anko_server =
CFLAGS_anko_server = $(CFLAGS)
