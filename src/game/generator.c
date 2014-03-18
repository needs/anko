#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <game/generator.h>
#include <game/simulator.h>
#include <game/board.h>

#define RANDOM_FLOAT() ((float)random()/RAND_MAX)

static enum tree_specie_t trees[] = { TS_OAK, TS_APPLE, TS_BIRCH, TS_PINE }; /* pine disabled for now */

typedef void(*spread_func)(board_t*, int, int, int*, int, int);

static int should_spawn(cell_type_t type, board_t *board, float density);
static void spawn_spot(spread_func spread, board_t *board, float density, float shatter_factor, int meta);
static void correct_water(board_t *board);
static void spread_lake(board_t *board, int x, int y, int *count, int size, int meta);
static void spread_forest(board_t *board, int x, int y, int *count, int size, int meta);
static void gen_stats(board_t *board);

board_t* generate(int width, int height, gen_params_t *params)
{
	board_t *board = NULL;
	int pos[2];
	int i, j;
	
	assert(width > 0);
	assert(height > 0);
	assert(params->tree_density <= 1);
   	assert(params->water_density <= 1);
	assert(params->water_shatter_factor > 0 && params->water_shatter_factor <= 1);
	
	if ((board = alloc_board(width, height)) == NULL) 
		return NULL;

	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			board->cells[i][j].type = CT_GRASS;
		}
	}
	
	while(should_spawn(CT_TREE, board, params->tree_density))
	{
		spawn_spot(&spread_forest, board, params->tree_density, 0.2, trees[random() % TS_TOTAL]);
	}
	
	while(should_spawn(CT_WATER, board, params->water_density))
	{
		spawn_spot(&spread_lake, board, params->water_density, params->water_shatter_factor, 0);
	}

	correct_water(board);
	
    pos[0] = random() % board->height;
    pos[1] = random() % board->width;

	board->cells[pos[0]][pos[1]].type = CT_TREE;
	board->cells[pos[0]][pos[1]].data.tree.life = 99;
	board->cells[pos[0]][pos[1]].data.tree.specie = TS_APPLE;

	gen_stats(board);
	return board;
}

static void gen_stats(board_t *board)
{
	int i,j;

	memset(&board->stats, 0, sizeof(board->stats));
	
	for(i = 0; i < board->height; i++)
		for(j = 0; j < board->width; j++)
	    {
		    if(board->cells[i][j].type == CT_TREE)
			{
				board->stats.total_tree++;
				if(board->cells[i][j].data.tree.life < 100)
				{
					if(board->cells[i][j].data.tree.life)
						board->stats.burning_tree++;
					else
						board->stats.burned_tree++;
				}
			}
			else if(board->cells[i][j].type == CT_WATER)
				board->stats.total_water++;
		}
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

static void spread_lake(board_t *board, int x, int y, int *count, int size, int meta)
{
	if(IS_OUT_OF_BOUNDS(board,x,y) || board->cells[y][x].type == CT_WATER)
		return;

	if(RANDOM_FLOAT() > (float)*count/size
	   && (get_neighbors_count(x,y, board, CT_WATER, NULL) != 1 || RANDOM_FLOAT() > 0.5))
	{
		board->cells[y][x].type = CT_WATER;
		*count = *count+1;
	    spread_lake(board, x+1, y, count, size, meta);
	    spread_lake(board, x-1, y, count, size, meta);
	    spread_lake(board, x, y+1, count, size, meta);
		spread_lake(board, x, y-1, count, size, meta);
	}
}

static void spread_forest(board_t *board, int x, int y, int *count, int size, int meta)
{
	if(IS_OUT_OF_BOUNDS(board,x,y) || board->cells[y][x].type == CT_TREE)
		return;
	
	if(RANDOM_FLOAT() > (float)*count/size
	   && (get_neighbors_count(x,y, board, CT_TREE, NULL) != 1 || RANDOM_FLOAT() > 0.5))
	{
		board->cells[y][x].type = CT_TREE;
		board->cells[y][x].data.tree.life = 100;
		if(RANDOM_FLOAT() < 0.6)
			board->cells[y][x].data.tree.specie = trees[meta];
		else
			board->cells[y][x].data.tree.specie = trees[random() % TS_TOTAL];
		
		*count = *count+1;
		spread_forest(board, x+1, y, count, size, meta);
		spread_forest(board, x-1, y, count, size, meta);
		spread_forest(board, x, y+1, count, size, meta);
		spread_forest(board, x, y-1, count, size, meta);
	}
}

static void spawn_spot(spread_func spread,board_t *board, float density, float shatter_factor, int meta)
{
	float size = density*board->width*board->height*shatter_factor;
	int count = 0;
    spread(board, random()%board->width, random()%board->height, &count, size, meta);
}

