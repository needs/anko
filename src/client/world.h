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
	int active_player;
} world_t;


int create_world(world_t *world, game_t *game);
void update_world(world_t *world);
void refresh_world(world_t *world);
void render_world(world_t *world, camera_t *camera);
void render_world_particles(world_t *world, camera_t *camera);
void regen_map(world_t *world); // update map rendering

/* Set active player (used to center camera & move the player on input) */
void world_set_active_player(world_t *world, int pid);

/* For you safety, call this only if you playing from Bugarach */
void end_of_the_world(world_t *world);


#endif /* _WORLD_H_ */
