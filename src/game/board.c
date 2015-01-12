#include <game/board.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


int alloc_board(board_t *board, int width, int height)
{
	int i;

	assert(board != NULL);
	assert(width > 0);
	assert(height > 0);

	if ((board->cells = malloc(height * sizeof(*board->cells))) == NULL) {
		perror("malloc(board->cells)");
		goto err_height;
	}

	for (i = 0; i < height; i++) {
		if ((board->cells[i] = malloc(width * sizeof(**board->cells))) == NULL) {
			perror("malloc(board->cells[i])");
			goto err_width;
		}
	}

	board->width = width;
	board->height = height;

	return 1;

err_width:
	while (--i >= 0)
		free(board->cells[i]);
	free(board->cells);
err_height:
	return 0;
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
}

int get_neighbors_count(int x, int y, board_t* board, cell_type_t type, int(*predicate)(void*))
{
	int count = 0;
	int i, j;
	for (j = -1; j <= 1; j++)
		for (i = -1; i <= 1; i++)
			if ((i || j)
			    && !IS_OUT_OF_BOUNDS(board,x+i,y+j)
			    && board->cells[y+j][x+i].type == type
			    && (predicate == NULL || predicate(&board->cells[y+j][x+i].data)) )
				count++;
	return count;
}


int cmp_cell(cell_t *c1, cell_t *c2)
{
	assert(c1 != NULL);
	assert(c2 != NULL);

	if (c1->type != c2->type)
		return 0;
	if (c1->type == CT_TREE)
		return c1->data.tree.life == c2->data.tree.life;
	else
		return 1;
}
