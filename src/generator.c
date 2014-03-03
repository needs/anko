#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "generator.h"
#include "simulator.h"
#include "board.h"

#define RANDOM_FLOAT() ((float)random()/RAND_MAX)

static int should_spawn(state_t type, board_t *board, float density);
static void spawn_water(board_t *board, float density, float shatter_factor);

board_t* generate(int width, int height, float tree_density, float water_density, float water_shatter_factor)
{
	board_t *board = NULL;
	int i, j;
	int tree_count = 0;
	
	assert(width > 0);
	assert(height > 0);
	assert(tree_density<1);
   	assert(water_density<1);
	assert(water_shatter_factor > 0 && water_shatter_factor <= 1);
	
	if ((board = alloc_board(width, height)) == NULL)
		return NULL;

	for (i = 0; i < board->height; i++)
		for (j = 0; j < board->width; j++)
			board->cells[i][j] = -1;
	
	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			
			int too_much_trees = ((float) tree_count / (board->width*board->height)) > tree_density;
			if(RANDOM_FLOAT() < tree_density && !too_much_trees)
			{
				board->cells[i][j] = ST_BURNABLE;
				tree_count++;
			}
			else
				board->cells[i][j] = ST_GRASS;
		}
	}

	while(should_spawn(ST_WATER, board, water_density))
	{
		spawn_water(board, water_density, water_shatter_factor);
	}

	board->cells[random()%board->height][random()%board->width] = ST_BURNING;
	return board;
}

static int should_spawn(state_t type, board_t *board, float density)
{
	int i,j;
	int count = 0;
	for(i = 0; i < board->height; i++)
		for(j = 0; j < board->width; j++)
			if(board->cells[i][j] == type)
				count++;
	return (float)count/(board->width*board->height) < density;
}

static void extend_water(board_t *board, int x, int y, int *count, int size)
{
	if(IS_OUT_OF_BOUNDS(board,x,y) || board->cells[y][x] == ST_WATER)
		return;

	if(RANDOM_FLOAT() > (float)*count/size)
	{
		board->cells[y][x] = ST_WATER;
		*count = *count+1;
		extend_water(board, x+1, y, count, size);
		extend_water(board, x-1, y, count, size);
		extend_water(board, x, y+1, count, size);
		extend_water(board, x, y-1, count, size);
	}
}
static void spawn_water(board_t *board, float density, float shatter_factor)
{
	float size = ((float)random()/RAND_MAX)*density*board->width*board->height*shatter_factor;
	int count = 0;
	extend_water(board, random()%board->width, random()%board->height, &count, size);
}

