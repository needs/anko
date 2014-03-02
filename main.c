#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "display.h"
#include "generator.h"
#include "simulator.h"

static const int WIDTH = 20;
static const int HEIGHT = 20;


int main(void)
{
	state_t **board, **dest, **tmp;
	char c;
	
	srandom(time(NULL));
	
	if ((board = generate(WIDTH, HEIGHT)) == NULL)
		return EXIT_FAILURE;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		return EXIT_FAILURE;

	display(board, WIDTH, HEIGHT);
	
	while((c = getchar()) != EOF)
	{
		step(dest,board, WIDTH,HEIGHT);

		tmp = board;
		board = dest;
		dest = tmp;
		
		display(board, WIDTH, HEIGHT);
	}
	
	

	return EXIT_SUCCESS;
}
