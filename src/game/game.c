#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <game/game.h>
#include <game/board.h>
#include <game/generator.h>
#include <game/simulator.h>


#define PLAYER_SPEED 0.006 /* px/ms at normal zoom */

static void swap(void **p1, void **p2);
void get_spawn_coords(board_t *board, float *x, float *y);

static void proceed_player_move(player_t *p, game_t *game, long diff);
static void proceed_player_shoot(player_t *p, game_t *game, long diff);

int new_game(game_t *game, int width, int height, gen_params_t *params, long sim_speed)
{
	assert(game != NULL);
	assert(width > 0);
	assert(height > 0);
	assert(params != NULL);

	memset(game, 0, sizeof(*game));

	if (!generate(&game->board[0], width, height, params))
		goto err_board1;
	if (!alloc_board(&game->board[1], width, height))
		goto err_board2;
	game->current = &game->board[0];
	game->old = &game->board[1];

	game->sim_speed = sim_speed;
	game->sim_timer = sim_speed;
	game->player_count = 0;
	memcpy(&game->gen_params, params, sizeof(gen_params_t));

	return 1;

err_board2:
	free_board(&game->board[0]);
err_board1:
	return 0;
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

void get_player_pos(game_t *game, int pid, float *x, float *y)
{
	assert(game != NULL);
	assert(pid >= 0 && pid < MAX_PLAYERS);
	*x = game->players[pid].x;
	*y = game->players[pid].y;
}

void set_player_pos(game_t *game, int pid, float x, float y)
{
	assert(game != NULL);
	assert(pid >= 0 && pid < MAX_PLAYERS);
	game->players[pid].x = x;
    game->players[pid].y = y;
}

void set_player_moving(game_t *game, int pid, short moving)
{
	assert(game != NULL);
	assert(pid >= 0 && pid < MAX_PLAYERS);
	game->players[pid].is_moving = moving;
}

void set_player_shooting(game_t *game, int pid, short shooting)
{
	assert(game != NULL);
	assert(pid >= 0 && pid < MAX_PLAYERS);
	game->players[pid].is_shooting = shooting;
}

static void proceed_player_move(player_t *p, game_t *game, long diff)
{
	float x, y;

	if (!p->is_used || !p->is_moving)
	    return;

	x = 0;
	y = 0;

	if (p->dir & DIR_LEFT)
		x -= diff * PLAYER_SPEED;
	if (p->dir & DIR_RIGHT)
		x += diff * PLAYER_SPEED;
	if (p->dir & DIR_UP)
		y -= diff * PLAYER_SPEED;
	if (p->dir & DIR_DOWN)
		y += diff * PLAYER_SPEED;

	if(x && y)
	{
		y *= 0.5;
		x *= 0.5;
	}

	x += p->x;
	y += p->y;

	if (x >= 0 && x < game->current->width&&
		(game->current->cells[(int)p->y][(int)x].type == CT_GRASS
		 || (game->current->cells[(int)p->y][(int)x].type == CT_TREE &&
			 game->current->cells[(int)p->y][(int)x].data.tree.life <= 0)))
		p->x = x;
	if (y >= 0 && y < game->current->height &&
		(game->current->cells[(int)y][(int)p->x].type == CT_GRASS
		 ||(game->current->cells[(int)y][(int)p->x].type == CT_TREE &&
			game->current->cells[(int)y][(int)p->x].data.tree.life <= 0)))
		p->y = y;
}

static void proceed_player_shoot(player_t *p, game_t *game, long diff)
{
	float x, y;
	float i;

	if (!p->is_used || !p->is_shooting || !p->weapon.type)
	    return;

	x = 0;
	y = 0;

	if (p->dir & DIR_LEFT)
		x -= 1;
	if (p->dir & DIR_RIGHT)
		x += 1;
	if (p->dir & DIR_UP)
		y -= 1;
	if (p->dir & DIR_DOWN)
		y += 1;

	for(i = 0; i < p->weapon.range; i++)
	{
		float xx, yy;
		xx = p->x + x + (x > 0 ? i : -i);
		yy = p->y + y + (y > 0 ? i : -i);

		if(xx >= 0 && xx < game->current->width
		   && yy >= 0 && yy < game->current->height
		   && game->current->cells[(int)yy][(int)xx].type == CT_TREE)
		{
			switch(p->weapon.type)
			{
			case WP_FLAMETHROWER:
				game->current->cells[(int)yy][(int)xx].data.tree.life-=(float)diff/1000;
				break;
			case WP_WATERPISTOL:
				game->current->cells[(int)yy][(int)xx].data.tree.life+=(float)diff/1000;
				break;
			}
		}
	}

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
		proceed_player_move(&game->players[i], game, diff);
		proceed_player_shoot(&game->players[i], game, diff);
	}

	return 0;
}


void game_over(game_t *game)
{
	assert(game != NULL);

	free_board(game->current);
	free_board(game->old);
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
	game->players[pid].team = team;
	if(team == TEAM_ARBRIST)
	{
		game->players[pid].weapon.type = WP_WATERPISTOL;
		game->players[pid].weapon.range = 1;
	}
	else if(team == TEAM_BURNER)
	{
		game->players[pid].weapon.type = WP_FLAMETHROWER;
		game->players[pid].weapon.range = 3;
	}
	else
		game->players[pid].weapon.type = WP_NONE;

	game->players[pid].is_used = 1;
	game->players[pid].is_moving = 0;
	game->players[pid].is_shooting = 0;
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
	*x = *x + 0.5;
	*y = *y + 0.5;
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

int regenerate_map(game_t *game)
{
	board_t new;

	assert(game != NULL);

	if (!generate(&new, game->current->width, game->current->height, &game->gen_params))
		return 0;

	free_board(game->current);
	*game->current = new;
	return 1;
}

int teleport_player(game_t *game, int pid, int x, int y)
{
	assert(game);
	assert(game->current);
	if(x >= 0 && x < game->current->width
	   && y >= 0 && y < game->current->height
	   && game->current->cells[y][x].type == CT_GRASS)
	{
		set_player_pos(game, pid, x+0.5, y+0.5);
		return 1;
	}
	return 0;
}
