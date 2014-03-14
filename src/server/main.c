#include <stdio.h>
#include <stdlib.h>
#include <enet/enet.h>

#include <server/config.h>
#include <server/server.h>

int init_network();
void terminate_network();

int main(int argc, char** argv)
{
	server_t *server;
	if(!config_from_file("server.cfg", 1) == 2)
		return EXIT_FAILURE;
	if(config_from_args(argc, argv))
		return EXIT_FAILURE;

	if(!init_network())
		goto err_network;
	if((server = create_server(config.port)) == NULL)
	{
		perror("Can't create server\n");
		goto err_server;
	}

	
	terminate_network();
	return EXIT_SUCCESS;
err_server:
	terminate_network();
err_network:
	return EXIT_FAILURE;
}

int init_network()
{
	if(enet_initialize () != 0)
	{
		fprintf(stderr, "Can't init enet.\n");
		return 0;
	}
	return 1;
}

void terminate_network()
{
	enet_deinitialize();
}
