#ifndef _MAP_H_
#define _MAP_H_

#include <SDL2/SDL.h>
#include "../board.h"
#include "textures.h"


#define TILE_WIDTH  68
#define TILE_HEIGHT 34

typedef struct mapcell_t {
	int x, y;
	tex_t floor[TEX_TOTAL], entity[TEX_TOTAL];
} mapcell_t;

typedef struct map_t {
	int width, height;
	mapcell_t **cells;
} map_t;


map_t* create_map(int width, int height);
void free_map(map_t *map);
void render_map(map_t *map, board_t *board);


#endif /* _MAP_H_ */