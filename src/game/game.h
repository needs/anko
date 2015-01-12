#ifndef _GAME_H_
#define _GAME_H_

#include <game/board.h>
#include <game/generator.h>


#define MAX_PLAYERS 16


enum {
	TEAM_NONE = 0,		/* Spectator */
	TEAM_ARBRIST,
	TEAM_BURNER
};


enum {
	DIR_LEFT  = 1,
	DIR_UP    = 2,
	DIR_RIGHT = 4,
	DIR_DOWN  = 8
};

enum {
	WP_NONE = 0,
	WP_FLAMETHROWER,
	WP_WATERPISTOL
};

typedef struct weapon_t {
	int type;
	float range;
} weapon_t;

typedef struct player_t {
	short is_used;		/* 0 if this structure is not linked to a player */
	short is_shooting;
	short is_moving;
	int dir;
	int team;
	weapon_t weapon;
	float x, y;
} player_t;

typedef struct game_t {
	/* current and old point on one of the board in board[2] */
	board_t *current, *old;
	board_t board[2];

	player_t players[MAX_PLAYERS];
	int player_count;
	long sim_speed, sim_timer;
	gen_params_t gen_params;
} game_t;


int new_game(game_t *game, int width, int height, gen_params_t *params, long sim_speed);
void game_over(game_t *game);

/* Set player direction */
void set_player_dir(game_t *game, int pid, int dir);
int get_player_dir(game_t *game, int pid);

void get_player_pos(game_t *game, int pid, float *x, float *y);
void set_player_pos(game_t *game, int pid, float x, float y);
void set_player_moving(game_t *game, int pid, short moving);
void set_player_shooting(game_t *game, int pid, short shooting);

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

void regenerate_map(game_t *game);
int teleport_player(game_t *game, int pid, int x, int y);

#endif /* _GAME_H_ */
