#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "world.h"
#include "map.h"
#include "../board.h"


world_t* create_world(board_t *board)
{
	world_t *world;

	assert(board != NULL);

	if ((world = malloc(sizeof(*world))) == NULL) {
		perror("malloc(world)");
		goto err_world;
	}

	if ((world->map = create_map(board)) == NULL)
		goto err_map;
	if ((world->gen = init_particles()) == NULL)
		goto err_particles;

	return world;

err_particles:
	free_map(world->map);
err_map:
	free(world);
err_world:
	return NULL;
}


void update_world(world_t *world, board_t *current, board_t *old)
{
	assert(world != NULL);
	assert(current != NULL);
	assert(old != NULL);

	update_map(world->map, world->gen, current, old);
}


void render_world(world_t *world, camera_t *camera)
{
	assert(world != NULL);
	assert(camera != NULL);

	render_map(world->map, camera);
	render_particles(world->gen, camera);
}


void end_of_the_world(world_t *world)
{
	assert(world != NULL);

	free_map(world->map);
	free_particles(world->gen);
	free(world);
}
