#ifndef _SERVER_H_
#define _SERVER_H_

#include <enet/enet.h>
#include <game/game.h>

#define MAX_CONNECTIONS 32
#define MAX_CHANNELS 1

typedef struct server_t
{
	ENetAddress address;
	ENetHost * host;
	game_t* game;
	int is_running;
} server_t;

server_t *create_server(int port);
void destroy_server(server_t *server);
void server_tick(server_t *server, unsigned long diff);


#endif /* _SERVER_H_ */
