#ifndef _WORLD_H_
#define _WORLD_H_

#include <client/linmath.h>
#include <client/map.h>
#include <client/particles.h>
#include <game/game.h>
#include <game/board.h>


typedef struct world_t {
	map_t *map;
	partgen_t *gen;
	game_t *game;
} world_t;


world_t* create_world(game_t *game);
void update_world(world_t *world);
void render_world(world_t *world, camera_t *camera);

/* For you safety, call this only if you playing from Bugarach */
void end_of_the_world(world_t *world);


#endif /* _WORLD_H_ */
