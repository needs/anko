#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "context.h"
#include "render.h"
#include "tiles.h"
#include "../generator.h"
#include "../simulator.h"


static const int WIDTH  = 20;
static const int HEIGHT = 20;


int main(void)
{
	state_t **board, **dest, **tmp;

	srandom(time(NULL));

	if (!init_context())
		goto err_context;

	if ((board = generate(WIDTH, HEIGHT, 0.7,0.1)) == NULL)
		goto err_board;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		goto err_dest;

	while(1) {
		step(dest, board, WIDTH, HEIGHT);

		tmp = board;
		board = dest;
		dest = tmp;
		
		render(board, WIDTH, HEIGHT);
		SDL_Delay(1000);
	}

	close_context();

	return EXIT_SUCCESS;

err_dest:
	free_board(board, WIDTH, HEIGHT);
err_board:
	close_context();
err_context:
	return EXIT_FAILURE;
}
