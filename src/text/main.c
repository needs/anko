#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <text/display.h>
#include <game/generator.h>
#include <game/simulator.h>
#include <game/board.h>
#include <getopt.h>
#include <signal.h>

static const int WIDTH = 50;
static const int HEIGHT = 50;

void init_from_args(int argc, char **argv, int *speed, gen_params_t *params);
void usage(char** argv);

int is_running;


void handler(int sig)
{
	if(sig == SIGINT)
		is_running = 0;
}


int main(int argc, char **argv)
{
	board_t *board, *dest, *tmp;
	int speed = 500;
	gen_params_t gen_params = { .tree_density = 0.7, .water_density = 0.1, .water_shatter_factor = 0.4 };
	
	is_running = 1;
	init_from_args(argc, argv, &speed, &gen_params);
	
	srandom(time(NULL));

	signal(SIGINT,&handler);
	
	if ((board = generate(WIDTH, HEIGHT, gen_params)) == NULL)
		return EXIT_FAILURE;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		return EXIT_FAILURE;

	clear_screen();
	display(board);
	
	while(is_running)
	{
		step(dest, board);
		
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

void init_from_args(int argc, char **argv, int *speed, gen_params_t *params)
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
			params->tree_density = atof(optarg);
			break;
		case 'w':
			params->water_density = atof(optarg);
			break;
		case 'k':
			params->water_shatter_factor = atof(optarg);
			break;
		case 'h':
			usage(argv);
			exit(EXIT_SUCCESS);
		}
	}
}
