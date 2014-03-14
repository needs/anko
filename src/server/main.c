#include <stdio.h>
#include <stdlib.h>
#include <enet/enet.h>
#include <sys/time.h>

#include <server/config.h>
#include <server/server.h>

#define TICKS_PER_SECOND 60

int init_network();
void terminate_network();

int main(int argc, char** argv)
{
	server_t *server;
	struct timeval last_time, current_time;
	unsigned long diff;

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

	gettimeofday(&last_time, NULL);

	while(server->is_running)
	{
		gettimeofday(&current_time, NULL);
		diff = 1000 * (current_time.tv_sec - last_time.tv_sec) + (current_time.tv_usec - last_time.tv_usec) / 1000;

		server_tick(server, diff);

		last_time.tv_sec = current_time.tv_sec;
		last_time.tv_usec = current_time.tv_usec;
		usleep(((float)1000/TICKS_PER_SECOND)*1000);
		printf("%li\n", diff);
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
