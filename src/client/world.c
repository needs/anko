#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <client/world.h>
#include <client/map.h>
#include <game/game.h>
#include <game/board.h>


int create_world(world_t *world, game_t *game)
{
	assert(world != NULL);
	assert(game != NULL);

	memset(world, sizeof(*world), 0);
	if (!create_map(&world->map, game->current))
		goto err_map;
	if ((world->gen = init_particles()) == NULL)
		goto err_particles;
	world->game = game;
	world->active_player = -1;

	return 1;

err_particles:
	free_map(&world->map);
err_map:
	return 0;
}

int regen_map(world_t *world)
{
	map_t map;

	if (!create_map(&map, world->game->current))
		return 0;

	free_map(&world->map);
	world->map = map;
	return 1;
}

void refresh_world(world_t *world)
{
	assert(world != NULL);

	update_map(&world->map, world->gen, world->game->current, world->game->old);
	update_particles(world->gen);
}

void update_world(world_t *world)
{
	int i;
	assert(world != NULL);
	for(i = 0; i < world->game->player_count; i++)
	{
		player_t *p = &world->game->players[i];
		if(p->is_used && p->is_shooting)
		{
			float temp_speed = 500;
			struct partargs_t prop = PARTARGS_DEFAULT;
			float px, py, mx, my, mz;
			prop.lifetime = 0.25;
			prop.tex = TEX_PARTICLES_FIRE1;
			prop.box.start.x = 3.0;
			prop.box.start.y = 3.0;
			prop.box.end.x = 4.0;
			prop.box.end.y = 10.0;
			prop.spawn_period = 0.0;
			prop.dir.dispersion = 0.40;
			prop.dir.rotate = 1;

			prop.dir.y = 0;
			prop.dir.x = 0;

			if (p->dir & DIR_LEFT)
			{
				prop.dir.y -= temp_speed;
				prop.dir.x -= temp_speed;
			}
			if (p->dir & DIR_RIGHT)
			{
				prop.dir.y += temp_speed;
				prop.dir.x += temp_speed;
			}
			if (p->dir & DIR_DOWN)
			{
				prop.dir.y += temp_speed;
				prop.dir.x -= temp_speed;
			}
			if (p->dir & DIR_UP)
			{
				prop.dir.y -= temp_speed;
				prop.dir.x += temp_speed;
			}

			get_player_pos(world->game, world->active_player, &px, &py);
			get_map_coord(px, py, &mx, &my, &mz);
			spawn_particles(world->gen, 1, mx, my, mz, &prop);
		}
	}
}

void render_world(world_t *world, camera_t *camera)
{
	assert(world != NULL);
	assert(camera != NULL);

	render_map(&world->map, camera);
}

void render_world_particles(world_t *world, camera_t *camera)
{
	assert(world != NULL);
	assert(camera != NULL);

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

	free_map(&world->map);
	free_particles(world->gen);
}
