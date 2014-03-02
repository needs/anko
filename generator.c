#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "generator.h"
#include "simulator.h"

static state_t **alloc_board(int width, int height);

state_t** generate(int width, int height)
{
	state_t **board = NULL;
	int i, j;

	assert(width > 0);
	assert(height > 0);

	if ((board = alloc_board(width, height)) == NULL)
		return NULL;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			board[i][j] = random() % NB_STATE;
		}
	}

	return board;
}


static state_t **alloc_board(int width, int height)
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
