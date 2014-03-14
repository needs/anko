#include <stdio.h>
#include <server/server.h>
#include <server/config.h>

server_t *create_server(int port)
{
	server_t *server = malloc(sizeof(server_t));
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
	return NULL;
}

void destroy_server(server_t *server)
{
	game_over(server->game);
	enet_host_destroy(server->host);
	free(server);
}
