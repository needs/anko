#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <game/game.h>
#include <game/board.h>
#include <game/generator.h>
#include <game/simulator.h>


#define PLAYER_SPEED 500.0


static void swap(void **p1, void **p2);
void get_spawn_coords(board_t *board, float *x, float *y);


game_t* new_game(int width, int height, gen_params_t *params, long sim_speed)
{
	game_t *game;

	assert(width > 0);
	assert(height > 0);
	assert(params != NULL);

	if ((game = calloc(1, sizeof(*game))) == NULL) {
		perror("malloc(game)");
		goto err_game;
	}

	if ((game->current = generate(width, height, params)) == NULL)
		goto err_current;
	if ((game->old = alloc_board(width, height)) == NULL)
		goto err_old;

	game->sim_speed = sim_speed;
	game->sim_timer = sim_speed;
	game->player_count = 0;

	return game;

err_old:
	free_board(game->current);
err_current:
	free(game);
err_game:
	return NULL;
}


void set_player_dir(game_t *game, int pid, int dir)
{
	assert(game != NULL);
	assert(pid >= 0 && pid < MAX_PLAYERS);

	game->players[pid].dir = dir;
}


int get_player_dir(game_t *game, int pid)
{
	assert(game != NULL);
	assert(pid >= 0 && pid < MAX_PLAYERS);
	return game->players[pid].dir;
}


int update_game(game_t *game, long diff)
{
	int i;

	assert(game != NULL);

	/* Simulation */
	if (game->sim_timer < diff) {
		step(game->old, game->current);
		swap((void**)&game->current, (void**)&game->old);
		game->sim_timer = game->sim_speed;
		return 1;
	}
	game->sim_timer -= diff;

	/* Player movement */
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (!game->players[i].is_used)
			continue;

		if (game->players[i].dir & DIR_LEFT)
			game->players[i].x += diff * (1 / PLAYER_SPEED);
		if (game->players[i].dir & DIR_RIGHT)
			game->players[i].x -= diff * (1 / PLAYER_SPEED);
		if (game->players[i].dir & DIR_UP)
			game->players[i].y -= diff * (1 / PLAYER_SPEED);
		if (game->players[i].dir & DIR_DOWN)
			game->players[i].y += diff * (1 / PLAYER_SPEED);
	}
	
	return 0;
}


void game_over(game_t *game)
{
	assert(game != NULL);

	free_board(game->current);
	free_board(game->old);
	free(game);
}


/*
 * Add a player.
 * Return a player ID if success, player IDs are > 0.
 * Return -1 if maximum of players is reached.
 * Return -2 if teams must be balanced.
 */
int add_player(game_t *game, int team)
{
	int pid;

	assert(game != NULL);
	assert(team == TEAM_ARBRIST || team == TEAM_BURNER);

	if (game->player_count == MAX_PLAYERS)
		return -1;

	for (pid = 0; game->players[pid].is_used && pid < MAX_PLAYERS; pid++)
		;

	assert(pid != MAX_PLAYERS);
	get_spawn_coords(game->current, &game->players[pid].x, &game->players[pid].y);
	fprintf(stderr, "x = %f, y = %f.\n", game->players[pid].x, game->players[pid].y);
	game->players[pid].team = team;
	game->players[pid].is_used = 1;
	game->player_count++;

	return pid;
}


void rem_player(game_t *game, int pid)
{
	assert(game != NULL);
	assert(game->player_count > 0);
	assert(pid > 0 && pid < MAX_PLAYERS);
	assert(game->players[pid].is_used == 1);

	/* Simple as that (for now ;) ) */
	game->players[pid].is_used = 0;
	game->player_count--;
}


void get_spawn_coords(board_t *board, float *x, float *y)
{
	assert(board != NULL);
	assert(x != NULL);
	assert(y != NULL);

	/* thta's really not well made, need to change this in the futur,
	 * maybe with precomputed spawn points or an other game mechanic. */
	do {
		*x = random() % board->width;
		*y = random() % board->height;
	} while (board->cells[(int)*y][(int)*x].type != CT_GRASS);
	*x = *x - 0.5;
	*y = *y - 0.5;
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
