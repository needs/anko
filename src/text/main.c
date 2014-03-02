#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "display.h"
#include "../generator.h"
#include "../simulator.h"

static const int WIDTH = 20;
static const int HEIGHT = 20;


int main(void)
{
	state_t **board, **dest, **tmp;

	srandom(time(NULL));

	if ((board = generate(WIDTH, HEIGHT)) == NULL)
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
		sleep(1);
	}

	free_board(board, WIDTH, HEIGHT);
	free_board(dest , WIDTH, HEIGHT);

	return EXIT_SUCCESS;
}
