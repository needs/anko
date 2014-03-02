#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "display.h"
#include "../generator.h"
#include "../simulator.h"
#include <getopt.h>

static const int WIDTH = 50;
static const int HEIGHT = 50;

void init_from_args(int argc, char **argv, int *speed, float *tree_density);

int main(int argc, char **argv)
{
	state_t **board, **dest, **tmp;
	int speed = 500;
	float tree_density = 0.70;
	
	init_from_args(argc, argv, &speed, &tree_density);

	srandom(time(NULL));
	
	if ((board = generate(WIDTH, HEIGHT,tree_density)) == NULL)
		return EXIT_FAILURE;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		return EXIT_FAILURE;

	clear_screen();
	display(board, WIDTH, HEIGHT);
	
	while(1)
	{
		step(dest,board, WIDTH,HEIGHT);

		tmp = board;
		board = dest;
		dest = tmp;
		
		display(board, WIDTH, HEIGHT);
		usleep(speed*1000);
	}
	
	

	return EXIT_SUCCESS;
}

void init_from_args(int argc, char **argv, int *speed, float *tree_density)
{
	int opt;
	static struct option long_options[] =
	{
		{"speed", required_argument, 0, 's'},
		{"tree-density", required_argument, 0, 't'},
		{NULL,0,NULL,0}
	};

	while((opt = getopt_long(argc,argv, "s:t:", long_options, NULL)) != -1)
	{
		switch(opt)
		{
		case 's':
			*speed = atoi(optarg);
			break;
		case 't':
			*tree_density = atof(optarg);
			break;
		}
	}
}
