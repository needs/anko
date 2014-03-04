#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "display.h"
#include "../generator.h"
#include "../simulator.h"
#include "../board.h"
#include <getopt.h>
#include <signal.h>

static const int WIDTH = 50;
static const int HEIGHT = 50;

void init_from_args(int argc, char **argv, int *speed, float *tree_density,float *water_density,float *water_shattering);
void usage(char** argv);

int is_running;

void handler(int sig)
{
	if(sig == SIGINT)
	{
		is_running = 0;
	}
}

int main(int argc, char **argv)
{
	board_t *board, *dest, *tmp;
	int speed = 500;
	float tree_density = 0.70;
	float water_density = 0.10;
	float water_shattering = 0.4;
	
	is_running = 1;
	init_from_args(argc, argv, &speed, &tree_density, &water_density, &water_shattering);
	
	srandom(time(NULL));

	signal(SIGINT,&handler);
	
	if ((board = generate(WIDTH, HEIGHT,tree_density, water_density,water_shattering)) == NULL)
		return EXIT_FAILURE;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		return EXIT_FAILURE;

	clear_screen();
	display(board);
	
	while(is_running)
	{
		step(dest,board);

		tmp = board;
		board = dest;
		dest = tmp;
		
		display(board);
		usleep(speed*1000);
	}

	free_board(board);
	free_board(dest);

	return EXIT_SUCCESS;
}

void usage(char** argv)
{
	printf("Usage: %s [OPTIONS]\n", argv[0]);
	printf("Options:\n");
	printf("\t-h, --help \t\t\t\tDisplay help page\n");
	printf("\t-s, --speed=speed in ms\t\t\tSimulation speed\n");
	printf("\t-t, --tree-density=density [0;1]\tTree density\n");
	printf("\t-w, --water-density=density [0;1]\tWater density\n");
	printf("\t-k, --water-shattering=shattering ]0;1]\tWater shattering 1 means no shatteing\n");
}

void init_from_args(int argc, char **argv, int *speed, float *tree_density, float *water_density, float *water_shattering)
{
	int opt;
	static struct option long_options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"speed", required_argument, 0, 's'},
		{"tree-density", required_argument, 0, 't'},
		{"water-density", required_argument, 0, 'w'},
		{"water-shattering", required_argument, 0, 'k'},
		{NULL,0,NULL,0}
	};

	while((opt = getopt_long(argc,argv, "s:t:w:hk:", long_options, NULL)) != -1)
	{
		switch(opt)
		{
		case 's':
			*speed = atoi(optarg);
			break;
		case 't':
			*tree_density = atof(optarg);
			break;
		case 'w':
			*water_density = atof(optarg);
			break;
		case 'k':
			*water_shattering = atof(optarg);
			break;
		case 'h':
			usage(argv);
			exit(EXIT_SUCCESS);
		}
	}
}
