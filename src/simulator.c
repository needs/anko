#include <stdlib.h>
#include <assert.h>
#include "simulator.h"
#include <stdio.h>

int is_burning(void* data)
{
	tree_data_t *tree = data;
	return tree->life < 100 && tree->life > 0;
}

static board_t *main_board, *copy_board;
static board_t **update;

// still here for compatibility
void step(board_t* dest, board_t* src)
{
	int i,j;
	int nb;
	
	assert(dest != NULL);
	assert(src != NULL);
	assert(src->width == dest->width);
	assert(src->height == dest->height);

	for(j = 0; j < src->height; j++)
	{
		for(i = 0; i < src->width; i++)
		{
			dest->cells[j][i].type = src->cells[j][i].type;
			dest->cells[j][i].data = src->cells[j][i].data;
			
			if(src->cells[j][i].type == CT_TREE
			   && (nb = get_neighbors_count(i, j, src, CT_TREE, &is_burning)) >= 1) // Compute nb only if needed
			{
				dest->cells[j][i].type = CT_TREE;
				// Replace next line with should_burn with more parameters like humidity etc ?
				if( src->cells[j][i].data.tree.life == 100)
				{
					if( ((float)random() / RAND_MAX ) < (float)nb/4)
						dest->cells[j][i].data.tree.life = 99; // FLAME UP THIS TREE
					else
						dest->cells[j][i].data.tree.life = 100;
				}
				else if(src->cells[j][i].data.tree.life > 0)
					dest->cells[j][i].data.tree.life = 0; // src->cells[j][i].data.tree.life - 20;
			}
			else
			{
				dest->cells[j][i].type = src->cells[j][i].type;
				dest->cells[j][i].data = src->cells[j][i].data;
				
				if(src->cells[j][i].type == CT_TREE && is_burning(&src->cells[j][i].data))
					dest->cells[j][i].data.tree.life = 0; //src->cells[j][i].data.tree.life - 20;
			}
		}
	}
}

void simulate()
{
	board_t *tmp;
	step(copy_board, main_board);
	tmp = main_board;
	main_board = copy_board;
	copy_board = tmp;
	*update = main_board;
}

int init_simulator(board_t **dest, int width, int height, gen_params_t params)
{
	update = dest;
	if ((main_board = generate(width,height, params)) == NULL)
		goto err_main;
	if ((copy_board = alloc_board(width, height)) == NULL)
		goto err_copy;
	*update = main_board;
	return 1;
	
err_copy:
	free_board(main_board);
err_main:
	return 0;
}

void terminate_simulator()
{
	free_board(main_board);
	free_board(copy_board);
}
