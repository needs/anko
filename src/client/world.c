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

void regen_map(world_t *world)
{
	map_t *new = create_map(world->game->current);
	if(new)
	{
		free_map(world->map);
		world->map = new;
	}
}

void refresh_world(world_t *world)
{
	assert(world != NULL);

	update_map(world->map, world->gen, world->game->current, world->game->old);
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
			float temp_speed = 125;
			struct partargs_t prop = PARTARGS_DEFAULT;
			float px, py, mx, my, mz;
			prop.lifetime = 0.5;
			prop.tex = TEX_PARTICLES_FIRE1;
			prop.box.start.x = 2.0;
			prop.box.start.y = 2.0;
			prop.box.end.x = 3.0;
			prop.box.end.y = 3.0;
			prop.spawn_period = 0.0;
			
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
