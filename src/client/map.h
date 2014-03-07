#ifndef _MAP_H_
#define _MAP_H_

#include "linmath.h"
#include "../board.h"
#include "camera.h"
#include "particles.h"


#define TILE_WIDTH  68
#define TILE_HEIGHT 34


typedef struct map_t map_t;


/* Allocate the map based on the generated board. NULL on error. */
map_t* create_map(board_t *board);
void free_map(map_t *map);

/* Change the map based on the differences between old and current. */
void update_map(map_t *map, partgen_t *gen, board_t *current, board_t *old);

/* Just draw the map, nothing is computed here. */
void render_map(map_t *map, camera_t *camera);

#endif /* _MAP_H_ */
