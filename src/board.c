#include "board.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

state_t **alloc_board(int width, int height)
{
	int i;

	assert(width > 0);
	assert(height > 0);

	state_t **board = NULL;
	if ((board = malloc(height * sizeof(*board))) == NULL) {
		perror("malloc(board)");
		goto err_height;
	}

	for (i = 0; i < height; i++) {
		if ((board[i] = malloc(width * sizeof(**board))) == NULL) {
			perror("malloc(board[i])");
			goto err_width;
		}
	}

	return board;

err_width:
	while (--i >= 0)
		free(board[i]);
	free(board);
err_height:
	return NULL;
}


void free_board(state_t **board, int width, int height)
{
	int i;

	assert(board != NULL);
	assert(width > 0);
	assert(height > 0);

	for (i = 0; i < height; i++)
		free(board[i]);
	free(board);
}

int get_neighbors_count(int x, int y, state_t** board, int width, int height, state_t type)
{
	int count = 0;
	int i,j;
	for(j = -1; j <=1; j++)
		for(i = -1; i <= 1; i++)
			if((i || j) && !IS_OUT_OF_BOUNDS(x+i,y+j, w,h) && board[y+j][x+i] == type)
				count++;
	
	return count;
}
