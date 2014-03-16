#include <stdio.h>
#include <server/server.h>
#include <server/config.h>

server_t *create_server(int port)
{
	server_t *server = malloc(sizeof(server_t));
	if(!server)
		goto err_server;
	server->is_running = 1;
	server->address.host = ENET_HOST_ANY;
	server->address.port = port;
	server->host = enet_host_create( &server->address, MAX_CONNECTIONS, MAX_CHANNELS, 0, 0);
	if(!server->host)
		goto err_host;

	if((server->game = new_game(config.board_width, config.board_height, &config.gen_params, config.sim_speed)) == NULL)
		goto err_game;

	   printf("Started server on port %i\n", port);
	return server;
err_game:
	destroy_server(server);
err_host:
	free(server);
err_server:
	return NULL;
}

void destroy_server(server_t *server)
{
	game_over(server->game);
	enet_host_destroy(server->host);
	free(server);
}

static void poll_network(server_t* server)
{
	ENetEvent e;
	while(enet_host_service(server->host, &e, 0) > 0)
	{
		switch(e.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("new client connected from %x\n", e.peer->address.host);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf("%li bytes of data received\n", e.packet->dataLength);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			printf("client disconnected\n");
			break;
		default:
			break;
		}
	}
}

void server_tick(server_t *server, unsigned long diff)
{
	update_game(server->game, diff);
	poll_network(server);
}
