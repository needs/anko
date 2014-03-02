#include "simulator.h"
#include <stdlib.h>

#define IS_OUT_OF_BOUNDS(x,y,w,h) (x < 0 || x >= width || y < 0 || y >= height)

static int get_burning_neighbors(int x, int y, state_t** board, int width, int height);

void step(state_t** dest, state_t** src, int width, int height)
{
	int i,j;
	int nb;
	
	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			nb = get_burning_neighbors(i,j, src, width, height); 
			if(nb >= 1)
			{
				switch(src[j][i])
				{
				case BURNABLE:
					// Replace next line with should_burn with more parameters like humidity etc ?
					if( ((float)random() / RAND_MAX ) > (float)nb/8)
						dest[j][i] = BURNING;
					break;
				case BURNING:
					dest[j][i] = BURNED;
					break;
				default:
					dest[j][i] = src[j][i];
					break;
				}
			}
			else
				dest[j][i] = src[j][i];
			
		}
	}
}



static int get_burning_neighbors(int x, int y, state_t** board, int width, int height)
{
	int count = 0;
	int i,j;
	for(j = -1; j <=1; j++)
		for(i = -1; i <= 1; i++)
			if((i || j) && !IS_OUT_OF_BOUNDS(x+i,y+j, w,h) && board[y+j][x+i] == BURNING)
				count++;
	
	return count;
}
