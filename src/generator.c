#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "generator.h"
#include "simulator.h"
#include "board.h"

#define RANDOM_FLOAT() ((float)random()/RAND_MAX)

static int should_spawn(cell_type_t type, board_t *board, float density);
static void spawn_water(board_t *board, float density, float shatter_factor);

board_t* generate(int width, int height, gen_params_t params)
{
	board_t *board = NULL;
	int i, j;
	int tree_count = 0;
	
	assert(width > 0);
	assert(height > 0);
	assert(params.tree_density<1);
   	assert(params.water_density<1);
	assert(params.water_shatter_factor > 0 && params.water_shatter_factor <= 1);
	
	if ((board = alloc_board(width, height)) == NULL)
		return NULL;

	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			
			int too_much_trees = ((float) tree_count / (board->width*board->height)) > params.tree_density;
			if(RANDOM_FLOAT() < params.tree_density && !too_much_trees)
			{
				board->cells[i][j].type = CT_TREE;
				board->cells[i][j].data.tree.life = 100;
				tree_count++;
			}
			else
				board->cells[i][j].type = CT_GRASS;
		}
	}

	while(should_spawn(CT_WATER, board, params.water_density))
	{
		spawn_water(board, params.water_density, params.water_shatter_factor);
	}

	int pos[2] = { random()%board->height, random()%board->width};
	board->cells[pos[0]][pos[1]].type = CT_TREE;
	board->cells[pos[0]][pos[1]].data.tree.life = 99;
	return board;
}

static int should_spawn(cell_type_t type, board_t *board, float density)
{
	int i,j;
	int count = 0;
	for(i = 0; i < board->height; i++)
		for(j = 0; j < board->width; j++)
			if(board->cells[i][j].type == type)
				count++;
	return (float)count/(board->width*board->height) < density;
}

static void extend_water(board_t *board, int x, int y, int *count, int size)
{
	if(IS_OUT_OF_BOUNDS(board,x,y) || board->cells[y][x].type == CT_WATER)
		return;

	if(RANDOM_FLOAT() > (float)*count/size
	   && (get_neighbors_count(x,y, board, CT_WATER, NULL) != 1 || RANDOM_FLOAT() > 0.5))
	{
		board->cells[y][x].type = CT_WATER;
		*count = *count+1;
		extend_water(board, x+1, y, count, size);
		extend_water(board, x-1, y, count, size);
		extend_water(board, x, y+1, count, size);
		extend_water(board, x, y-1, count, size);
	}
}
static void spawn_water(board_t *board, float density, float shatter_factor)
{
	float size = density*board->width*board->height*shatter_factor;
	int count = 0;
	extend_water(board, random()%board->width, random()%board->height, &count, size);
}

