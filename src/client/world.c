#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <client/world.h>
#include <client/map.h>
#include <game/game.h>
#include <game/board.h>


world_t* create_world(game_t *game)
{
	world_t *world;

	assert(game != NULL);

	if ((world = malloc(sizeof(*world))) == NULL) {
		perror("malloc(world)");
		goto err_world;
	}

	if ((world->map = create_map(game->current)) == NULL)
		goto err_map;
	if ((world->gen = init_particles()) == NULL)
		goto err_particles;
	world->game = game;
	world->active_player = -1;

	return world;

err_particles:
	free_map(world->map);
err_map:
	free(world);
err_world:
	return NULL;
}


void update_world(world_t *world)
{
	assert(world != NULL);

	update_map(world->map, world->gen, world->game->current, world->game->old);
	update_particles(world->gen);
}


void render_world(world_t *world, camera_t *camera)
{
	assert(world != NULL);
	assert(camera != NULL);

	render_map(world->map, camera);
	render_particles(world->gen, camera);
}


void world_set_active_player(world_t *world, int pid)
{
	assert(world != NULL);
	world->active_player = pid;
}


void end_of_the_world(world_t *world)
{
	assert(world != NULL);

	free_map(world->map);
	free_particles(world->gen);
	free(world);
}
