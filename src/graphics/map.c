#include <stdlib.h>
#include <assert.h>
#include "sprites.h"
#include "context.h"
#include "map.h"


static void seed_map(map_t *map);
static map_t* alloc_map(int width, int height);


map_t* create_map(int width, int height)
{
	map_t *map;

	assert(width > 0);
	assert(height > 0);

	if ((map = alloc_map(width, height)) == NULL)
		return NULL;
	seed_map(map);

	return map;
}


void free_map(map_t *map)
{
	int i;

	assert(map != NULL);

	for (i = 0; i < map->height; i++)
		free(map->cells[i]);
	free(map->cells);
	free(map);
}


static map_t* alloc_map(int width, int height)
{
	int i;
	map_t *map;

	assert(width > 0);
	assert(height > 0);

	if ((map = malloc(sizeof(*map))) == NULL) {
		perror("malloc(map)");
		goto err_map;
	}

	if ((map->cells = malloc(height * sizeof(*map->cells))) == NULL) {
		perror("malloc(map->cells)");
		goto err_height;
	}

	for (i = 0; i < height; i++) {
		if ((map->cells[i] = malloc(width * sizeof(**map->cells))) == NULL) {
			perror("malloc(map->cells[i])");
			goto err_width;
		}
	}

	map->width = width;
	map->height = height;

	return map;

err_width:
	while (--i >= 0)
		free(map->cells[i]);
	free(map->cells);
err_height:
	free(map);
err_map:
	return NULL;
}


/* Introduit de la variation dans les textures
 * Précalcule les coordonnées des sprites. */
static void seed_map(map_t *map)
{
	int i, j, k;

	assert(map != NULL);

	for (i = 0; i < map->height; i++) {
		for (j = 0; j < map->width; j++) {
			int seed = random();

			map->cells[i][j].x = j*-TILE_WIDTH/2 + i*TILE_WIDTH/2;
			map->cells[i][j].y = i*TILE_HEIGHT/2 + j*TILE_HEIGHT/2;
			
			for (k = 0; k < ST_TOTAL; k++) {
				/* Floor */
				if (k == ST_WATER)
					map->cells[i][j].floor[k] = sprites[SP_WATER];
				else
					map->cells[i][j].floor[k] = sprites[SP_GRASS];
				
				/* Entity */
				switch (k) {
				case ST_BURNABLE:
					if (seed % 50 == 0)
						map->cells[i][j].entity[k] = sprites[SP_TREE2];
					else if (seed % 50 > 0 && seed % 50 < 15)
						map->cells[i][j].entity[k] = sprites[SP_TREE3];
					else
						map->cells[i][j].entity[k] = sprites[SP_TREE];
						
					break;
				case ST_BURNED:
					if (seed % 50 == 0)
						map->cells[i][j].entity[k] = sprites[SP_BURNED_TREE2];
					else if (seed % 50 > 0 && seed % 50 < 15)
						map->cells[i][j].entity[k] = sprites[SP_BURNED_TREE3];
					else
						map->cells[i][j].entity[k] = sprites[SP_BURNED_TREE];
					break;
				case ST_BURNING:
					if (seed % 50 == 0)
						map->cells[i][j].entity[k] = sprites[SP_BURNING_TREE2];
					else if (seed % 50 > 0 && seed % 50 < 15)
						map->cells[i][j].entity[k] = sprites[SP_BURNING_TREE3];
					else
						map->cells[i][j].entity[k] = sprites[SP_BURNING_TREE];
					break;
				default:
					map->cells[i][j].entity[k] = NULL;
				}
			}
		}
	}
}
