#include "board.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

board_t* alloc_board(int width, int height)
{
	int i;

	assert(width > 0);
	assert(height > 0);

	board_t * board = malloc(sizeof(board_t));
	board->width = width;
	board->height = height;
	
	if ((board->cells = malloc(height * sizeof(*board->cells))) == NULL) {
		perror("malloc(board)");
		goto err_height;
	}

	for (i = 0; i < height; i++) {
		if ((board->cells[i] = malloc(width * sizeof(**board->cells))) == NULL) {
			perror("malloc(board[i])");
			goto err_width;
		}
	}

	return board;

err_width:
	while (--i >= 0)
		free(board->cells[i]);
	free(board->cells);
err_height:
	return NULL;
}

void free_board(board_t *board)
{
	int i;

	assert(board != NULL);
	assert(board->cells != NULL);
	assert(board->width > 0);
	assert(board->height > 0);

	for (i = 0; i < board->height; i++)
		free(board->cells[i]);
	free(board->cells);
	free(board);
}

int get_neighbors_count(int x, int y, board_t* board, state_t type)
{
	int count = 0;
	int i,j;
	for(j = -1; j <=1; j++)
		for(i = -1; i <= 1; i++)
			if((i || j) && !IS_OUT_OF_BOUNDS(board,x+i,y+j) && board->cells[y+j][x+i] == type)
				count++;
	return count;
}
