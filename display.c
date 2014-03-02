#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "display.h"
#include "simulator.h"

static void clear_screen(void);
static char statemap[] = {' ', 'T', '#', '.'};

void display(state_t **board, int width, int height)
{
	int i, j;

	assert(board != NULL);
	assert(height > 0);
	assert(width > 0);

	clear_screen();
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			putchar(statemap[board[i][j]]);
		}
		putchar('\n');
	}
}

static void clear_screen(void)
{
	printf("\033[2J");
}
