#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "display.h"
#include "../simulator.h"

static void reset_screen(void);

void display(board_t *board)
{
	int i, j;
	static char statemap[][12] = {"\x1B[00m ", "\x1B[34m~", "\x1B[37m+", "\x1B[32mT", "\x1B[31m#", "\x1B[00m."};

	assert(board != NULL);
	assert(board->height > 0);
	assert(board->width > 0);

	reset_screen();
	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			printf(statemap[board->cells[i][j]]);
		}
		putchar('\n');
	}
}

void clear_screen(void)
{
	printf("\033[2J");
}

static void reset_screen(void)
{
	printf("\033[1;1H");
}
