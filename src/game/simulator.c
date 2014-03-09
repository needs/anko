#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <game/generator.h>
#include <game/simulator.h>

static int is_burning(void *data)
{
	tree_data_t *tree = data;
	return tree->life < 100 && tree->life > 0;
}


void step(board_t *dest, board_t *src)
{
	int i,j;
	int nb;
	
	assert(dest != NULL);
	assert(src != NULL);
	assert(src->width == dest->width);
	assert(src->height == dest->height);

	for (j = 0; j < src->height; j++)
	{
		for (i = 0; i < src->width; i++)
		{
			dest->cells[j][i].type = src->cells[j][i].type;
			dest->cells[j][i].data = src->cells[j][i].data;
			
			if (src->cells[j][i].type == CT_TREE
			   && (nb = get_neighbors_count(i, j, src, CT_TREE, &is_burning)) >= 1) // Compute nb only if needed
			{
				dest->cells[j][i].type = CT_TREE;
				// Replace next line with should_burn with more parameters like humidity etc ?
				if (src->cells[j][i].data.tree.life == 100)
				{
					if( ((float)random() / RAND_MAX ) < (float)nb/4)
						dest->cells[j][i].data.tree.life = 99; // FLAME UP THIS TREE
					else
						dest->cells[j][i].data.tree.life = 100;
				}
				else if (src->cells[j][i].data.tree.life > 0)
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
