#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "generator.h"
#include "simulator.h"
#include "board.h"

state_t** generate(int width, int height, float tree_density, float water_density)
{
	state_t **board = NULL;
	int i, j;

	int water_count = 0;
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
			
			float rand = (float)random() / RAND_MAX;

			// we augment the probabillity of water if in "lake"
			float should_more_water = (float)get_neighbors_count(j,i,board,width,height,ST_WATER)/16;

			// soft threshold
			int too_much_water = ((float) water_count / (width*height)) >  water_density+water_density/4;
			int too_much_trees = ((float) tree_count / (width*height)) > tree_density;
			
			if(rand < (water_density + should_more_water) && !too_much_water )
			{
				board[i][j] = ST_WATER;
				water_count++;
			}
			else if(rand < tree_density && !too_much_trees)
			{
				board[i][j] = ST_BURNABLE;
				tree_count++;
			}
			else
				board[i][j] = ST_GRASS;
		}
	}

	board[random()%height][random()%width] = ST_BURNING;
	
	return board;
}

