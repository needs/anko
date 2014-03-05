#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "generator.h"
#include "simulator.h"
#include "board.h"

#define RANDOM_FLOAT() ((float)random()/RAND_MAX)

typedef void(*spread_func)(board_t*,int,int,int*,int);

static int should_spawn(cell_type_t type, board_t *board, float density);
static void spawn_spot(spread_func spread, board_t *board, float density, float shatter_factor);
static void correct_water(board_t *board);
static void spread_lake(board_t *board, int x, int y, int *count, int size);
static void spread_forest(board_t *board, int x, int y, int *count, int size);

board_t* generate(int width, int height, gen_params_t params)
{
	board_t *board = NULL;
	int i, j;
	
	assert(width > 0);
	assert(height > 0);
	assert(params.tree_density<1);
   	assert(params.water_density<1);
	assert(params.water_shatter_factor > 0 && params.water_shatter_factor <= 1);
	
	if ((board = alloc_board(width, height)) == NULL) 
		return NULL;

	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			board->cells[i][j].type = CT_GRASS;
		}
	}
	
	while(should_spawn(CT_TREE, board, params.tree_density))
	{
		spawn_spot(&spread_forest, board, params.water_density, 0.5);
	}
	
	while(should_spawn(CT_WATER, board, params.water_density))
	{
		spawn_spot(&spread_lake, board, params.water_density, params.water_shatter_factor);
	}

	correct_water(board);
	
	int pos[2] = { random()%board->height, random()%board->width };
	board->cells[pos[0]][pos[1]].type = CT_TREE;
	board->cells[pos[0]][pos[1]].data.tree.life = 99;
	return board;
}

static void correct_water(board_t *board)
{
	int i,j;
	for(i = 0; i < board->height; i++)
		for(j = 0; j < board->width; j++)
			if(board->cells[i][j].type != CT_WATER
			   && get_neighbors_count(j,i, board, CT_WATER, NULL) >= 7)
				board->cells[i][j].type = CT_WATER;
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

static void spread_lake(board_t *board, int x, int y, int *count, int size)
{
	if(IS_OUT_OF_BOUNDS(board,x,y) || board->cells[y][x].type == CT_WATER)
		return;

	if(RANDOM_FLOAT() > (float)*count/size
	   && (get_neighbors_count(x,y, board, CT_WATER, NULL) != 1 || RANDOM_FLOAT() > 0.5))
	{
		board->cells[y][x].type = CT_WATER;
		*count = *count+1;
	    spread_lake(board, x+1, y, count, size);
	    spread_lake(board, x-1, y, count, size);
	    spread_lake(board, x, y+1, count, size);
		spread_lake(board, x, y-1, count, size);
	}
}

static void spread_forest(board_t *board, int x, int y, int *count, int size)
{
	if(IS_OUT_OF_BOUNDS(board,x,y) || board->cells[y][x].type == CT_TREE)
		return;

	if(RANDOM_FLOAT() > (float)*count/size
	   && (get_neighbors_count(x,y, board, CT_TREE, NULL) != 1 || RANDOM_FLOAT() > 0.5))
	{
		board->cells[y][x].type = CT_TREE;
		board->cells[y][x].data.tree.life = 100;
		
		*count = *count+1;
		spread_forest(board, x+1, y, count, size);
		spread_forest(board, x-1, y, count, size);
		spread_forest(board, x, y+1, count, size);
		spread_forest(board, x, y-1, count, size);
	}
}

static void spawn_spot(spread_func spread,board_t *board, float density, float shatter_factor)
{
	float size = density*board->width*board->height*shatter_factor;
	int count = 0;
    spread(board, random()%board->width, random()%board->height, &count, size);
}

