#ifndef _WORLD_H_
#define _WORLD_H_

#include "linmath.h"
#include "map.h" 
#include "particles.h"
#include "../board.h"


typedef struct world_t {
	map_t *map;
	partgen_t *gen;
} world_t;


world_t* create_world(board_t *board);
void update_world(world_t *world, board_t *current, board_t *old);
void render_world(world_t *world, camera_t *camera);

/* For you safety, call this only if you playing from Bugarach */
void end_of_the_world(world_t *world);


#endif /* _WORLD_H_ */
