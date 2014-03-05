#ifndef _MAP_H_
#define _MAP_H_

#include "linmath.h"
#include "../board.h"
#include "textures.h"


#define TILE_WIDTH  68
#define TILE_HEIGHT 34

typedef struct mapcell_t {
	mat4x4 model;
	int seed;
} mapcell_t;

typedef struct map_t {
	int width, height;
	mapcell_t **cells;
} map_t;


map_t* create_map(int width, int height);
void free_map(map_t *map);
void render_map(map_t *map, board_t *board);


#endif /* _MAP_H_ */
