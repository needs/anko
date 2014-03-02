#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "display.h"
#include "../simulator.h"

static void reset_screen(void);

void display(state_t **board, int width, int height)
{
	int i, j;
	static char statemap[][12] = {"\x1B[00m ", "\x1B[34m~", "\x1B[37m+", "\x1B[32mT", "\x1B[31m#", "\x1B[00m."};

	assert(board != NULL);
	assert(height > 0);
	assert(width > 0);

	reset_screen();
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			printf(statemap[board[i][j]]);
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
