#ifndef _GAME_H_
#define _GAME_H_

#include <game/board.h>
#include <game/generator.h>


typedef struct game_t {
	board_t *current, *old;
	long sim_speed, sim_timer;
} game_t;


game_t* new_game(int width, int height, gen_params_t *params, long sim_speed);
void game_over(game_t *game);

/* Return 1 when something change, 0 else */
int  update_game(game_t *game, long diff);


#endif /* _GAME_H_ */
