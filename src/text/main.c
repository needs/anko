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

void init_from_args(int argc, char **argv, int *speed, float *tree_density,float *water_density );
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
	state_t **board, **dest, **tmp;
	int speed = 500;
	float tree_density = 0.70;
	float water_density = 0.10;
	
	is_running = 1;
	init_from_args(argc, argv, &speed, &tree_density, &water_density);
	
	srandom(time(NULL));

	signal(SIGINT,&handler);
	
	if ((board = generate(WIDTH, HEIGHT,tree_density, water_density)) == NULL)
		return EXIT_FAILURE;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		return EXIT_FAILURE;

	clear_screen();
	display(board, WIDTH, HEIGHT);
	
	while(is_running)
	{
		step(dest,board, WIDTH,HEIGHT);

		tmp = board;
		board = dest;
		dest = tmp;
		
		display(board, WIDTH, HEIGHT);
		usleep(speed*1000);
	}

	free_board(board, WIDTH, HEIGHT);
	free_board(dest , WIDTH, HEIGHT);

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
}

void init_from_args(int argc, char **argv, int *speed, float *tree_density, float *water_density)
{
	int opt;
	static struct option long_options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"speed", required_argument, 0, 's'},
		{"tree-density", required_argument, 0, 't'},
		{"water-density", required_argument, 0, 'w'},		
		{NULL,0,NULL,0}
	};

	while((opt = getopt_long(argc,argv, "s:t:w:h", long_options, NULL)) != -1)
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
		case 'h':
			usage(argv);
			exit(EXIT_SUCCESS);
		}
	}
}
