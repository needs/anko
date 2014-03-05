#ifndef _MAP_H_
#define _MAP_H_

#include "linmath.h"
#include "../board.h"


#define TILE_WIDTH  68
#define TILE_HEIGHT 34

typedef struct map_t map_t;

map_t* create_map(board_t *board);
void free_map(map_t *map);
void render_map(map_t *map, board_t *board);


#endif /* _MAP_H_ */
