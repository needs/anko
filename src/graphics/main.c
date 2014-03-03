#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "context.h"
#include "event.h"
#include "render.h"
#include "../board.h"
#include "../generator.h"
#include "../simulator.h"


static const int FPS = 60;
static const int WIDTH  = 20;
static const int HEIGHT = 20;


int main(void)
{
	board_t *board, *dest, *tmp;
	map_t *map;
	int frame = 0;

	srandom(time(NULL));

	if (!init_context())
		goto err_context;

	if ((board = generate(WIDTH, HEIGHT, 0.7, 0.1, 0.4)) == NULL)
		goto err_board;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		goto err_dest;
	if ((map = create_map(WIDTH, HEIGHT)) == NULL)
		goto err_map;

	while(!quit) {
		process_events();
		if (frame % FPS == 0) {
			step(dest, board);
			tmp = board;
			board = dest;
			dest = tmp;
			frame = 1;
		} else {
			frame++;
		}

		render(board, map);
		SDL_Delay(1 / FPS);
	}

	free_map(map);
	free_board(dest);
	free_board(board);
	close_context();

	return EXIT_SUCCESS;

err_map:
	free_board(dest);
err_dest:
	free_board(board);
err_board:
	close_context();
err_context:
	return EXIT_FAILURE;
}
