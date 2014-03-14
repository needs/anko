#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <client/world.h>

int is_command(char *s);
char* execute_command(char *s, world_t *world);

#endif /* _COMMANDS_H_ */
