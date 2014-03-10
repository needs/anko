#ifndef _PLAYERS_H_
#define _PLAYERS_H_

#include <game/game.h>
#include <client/camera.h>

void init_players_rendering(void);
void render_players(player_t *players, int player_count, camera_t *camera);
void terminate_players_rendering(void);

#endif /* _PLAYERS_H_ */
