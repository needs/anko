#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <game/game.h>
#include <game/board.h>
#include <game/generator.h>
#include <game/simulator.h>


static void swap(void **p1, void **p2);


game_t* new_game(int width, int height, gen_params_t *params, long sim_speed)
{
	game_t *game;

	assert(width > 0);
	assert(height > 0);
	assert(params != NULL);

	if ((game = malloc(sizeof(*game))) == NULL) {
		perror("malloc(game)");
		goto err_game;
	}

	if ((game->current = generate(width, height, *params)) == NULL)
		goto err_current;
	if ((game->old = alloc_board(width, height)) == NULL)
		goto err_old;

	game->sim_speed = sim_speed;
	game->sim_timer = sim_speed;

	return game;

err_old:
	free_board(game->current);
err_current:
	free(game);
err_game:
	return NULL;
}


int update_game(game_t *game, long diff)
{
	assert(game != NULL);

	if (game->sim_timer < diff) {
		step(game->old, game->current);
		swap((void**)&game->current, (void**)&game->old);
		game->sim_timer = game->sim_speed;
		return 1;
	}
	game->sim_timer -= diff;
	return 0;
}


void game_over(game_t *game)
{
	assert(game != NULL);

	free_board(game->current);
	free_board(game->old);
	free(game);
}


static void swap(void **p1, void **p2)
{
	void *tmp;

	assert(p1 != NULL);
	assert(p2 != NULL);
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}
