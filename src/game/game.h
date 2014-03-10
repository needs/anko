#ifndef _GAME_H_
#define _GAME_H_

#include <game/board.h>
#include <game/generator.h>


#define MAX_PLAYERS 16


enum {
	TEAM_NONE = 0,		/* Spectator */
	TEAM_ARBRIST,
	TEAM_BURNER,
};


enum {
	DIR_LEFT  = 1,
	DIR_TOP   = 2,
	DIR_RIGHT = 4,
	DIR_DOWN  = 8,
};


typedef struct player_t {
	short is_used;		/* 0 if this structure is not linked to a player */

	int dir;
	int team;
	float x, y;
} player_t;

typedef struct game_t {
	board_t *current, *old;
	player_t players[MAX_PLAYERS];
	int player_count;

	long sim_speed, sim_timer;
} game_t;


game_t* new_game(int width, int height, gen_params_t *params, long sim_speed);
void game_over(game_t *game);

/* Return 1 when something change, 0 else */
int  update_game(game_t *game, long diff);

/*
 * Add a player.
 * Return a player ID if success, player IDs are > 0.
 * Return -1 if maximum of players is reached.
 * Return -2 if teams must be balanced.
 */
int add_player(game_t *game, int team);

/* Remove a player of the board */
void rem_player(game_t *game, int pid);


#endif /* _GAME_H_ */
