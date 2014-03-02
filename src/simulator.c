#include "simulator.h"
#include <stdlib.h>

void step(state_t** dest, state_t** src, int width, int height)
{
	int i,j;
	int nb;
	
	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			nb = get_neighbors_count(i,j, src, width, height, ST_BURNING); 
			if(nb >= 1)
			{
				switch(src[j][i])
				{
				case ST_BURNABLE:
					// Replace next line with should_burn with more parameters like humidity etc ?
					if( ((float)random() / RAND_MAX ) < (float)nb/2.5)
						dest[j][i] = ST_BURNING;
					break;
				case ST_BURNING:
					dest[j][i] = ST_BURNED;
					break;
				default:
					dest[j][i] = src[j][i];
					break;
				}
			}
			else
			{
				if(src[j][i] == ST_BURNING)
					dest[j][i] = ST_BURNED;
				else
					dest[j][i] = src[j][i];
			}
		}
	}
}

