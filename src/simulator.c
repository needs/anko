#include "simulator.h"
#include <stdlib.h>

void step(board_t* dest, board_t* src)
{
	int i,j;
	int nb;
	
	for(j = 0; j < src->height; j++)
	{
		for(i = 0; i < src->width; i++)
		{
			nb = get_neighbors_count(i,j, src, ST_BURNING); 
			if(nb >= 1)
			{
				switch(src->cells[j][i])
				{
				case ST_BURNABLE:
					// Replace next line with should_burn with more parameters like humidity etc ?
					if( ((float)random() / RAND_MAX ) < (float)nb/2.5)
						dest->cells[j][i] = ST_BURNING;
					break;
				case ST_BURNING:
					dest->cells[j][i] = ST_BURNED;
					break;
				default:
					dest->cells[j][i] = src->cells[j][i];
					break;
				}
			}
			else
			{
				if(src->cells[j][i] == ST_BURNING)
					dest->cells[j][i] = ST_BURNED;
				else
					dest->cells[j][i] = src->cells[j][i];
			}
		}
	}
}

