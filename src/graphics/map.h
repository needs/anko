#ifndef _MAP_H_
#define _MAP_H_

#include <SDL2/SDL.h>
#include "../board.h"


typedef struct mapcell_t {
	int x, y;
	SDL_Texture *floor[ST_TOTAL], *entity[ST_TOTAL];
} mapcell_t;

typedef struct map_t {
	int width, height;
	mapcell_t **cells;
} map_t;


map_t* create_map(int width, int height);
void free_map(map_t *map);


#endif /* _MAP_H_ */
