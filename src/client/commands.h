#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <game/game.h>

int is_command(char *s);
char* execute_command(char *s, game_t *game);

#endif /* _COMMANDS_H_ */
