#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "generator.h"
#include "simulator.h"
#include "board.h"

#define RANDOM_FLOAT() ((float)random()/RAND_MAX)

static int should_spawn(state_t type, state_t **board, int width, int height, float density);
static void extend_water(state_t **board, int width, int height ,int x, int y, float water_density);

state_t** generate(int width, int height, float tree_density, float water_density)
{
	state_t **board = NULL;
	int i, j;
	int tree_count = 0;
	
	assert(width > 0);
	assert(height > 0);

	if ((board = alloc_board(width, height)) == NULL)
		return NULL;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			board[i][j] = -1;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			int too_much_trees = ((float) tree_count / (width*height)) > tree_density;

			if(RANDOM_FLOAT() < tree_density && !too_much_trees)
			{
				board[i][j] = ST_BURNABLE;
				tree_count++;
			}
			else
				board[i][j] = ST_GRASS;
		}
	}

	while(should_spawn(ST_WATER, board, width,height,water_density))
	{
		extend_water(board, width, height,random()%width,random()%height, water_density);
	}

	board[random()%height][random()%width] = ST_BURNING;
	return board;
}

static int should_spawn(state_t type, state_t**board, int width, int height, float density)
{
	int i,j;
	int count = 0;
	for(i = 0; i < height; i++)
		for(j = 0; j < width; j++)
			if(board[i][j] == type)
				count++;
	return (float)count/(width*height) < density;
}

static void extend_water_rec(state_t **board, int width, int height, int x, int y, int *count, int size)
{
	if(IS_OUT_OF_BOUNDS(x,y,width,height) || board[y][x] == ST_WATER)
		return;
	
	if(RANDOM_FLOAT() > (float)*count/size)
	{
		board[y][x] = ST_WATER;
		*count = *count+1;
		extend_water_rec(board, width, height, x+1, y, count, size);
		extend_water_rec(board, width, height, x-1, y, count, size);
		extend_water_rec(board, width, height, x, y+1, count, size);
		extend_water_rec(board, width, height, x, y-1, count, size);
	}
}
static void extend_water(state_t **board, int width, int height,int x, int y, float water_density)
{
	float size = ((float)random()/RAND_MAX) * water_density *width*height;
	int water_count = 0;
	extend_water_rec(board,width,height, x,y, &water_count, size);
}

