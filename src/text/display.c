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
			switch(board->cells[i][j].type)
			{
			case CT_GRASS:
				printf(statemap[0]);
				break;
			case CT_WATER:
				printf(statemap[1]);
				break;
			case CT_ROCK:
				printf(statemap[2]);
				break;
			case CT_TREE:
				if(board->cells[i][j].data.tree.life == 100)
					printf(statemap[3]);
				else if(board->cells[i][j].data.tree.life)
					printf(statemap[4]);
				else
					printf(statemap[5]);
				break;
			default:
				break;
			}
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
