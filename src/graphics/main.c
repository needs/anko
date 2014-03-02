#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "render.h"
#include "../generator.h"
#include "../simulator.h"


static const int WIDTH = 20;
static const int HEIGHT = 20;


int main(void)
{
	SDL_Window *window;
	SDL_Surface **tiles;
	state_t **board, **dest, **tmp;
	
	srandom(time(NULL));
	
	if ((board = generate(WIDTH, HEIGHT)) == NULL)
		goto err_board;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		goto err_dest;

	SDL_Init(SDL_INIT_VIDEO);

	if ((tiles = load_tiles()) == NULL)
		goto err_tiles;

	window = SDL_CreateWindow("Anko",
				   SDL_WINDOWPOS_UNDEFINED, // x
				   SDL_WINDOWPOS_UNDEFINED, // y
				   640,			    // Width
				   480,			    // Height
				   SDL_WINDOW_SHOWN);

	if (!window) {
		fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
		goto err_window;
	}

	while(1) {
		step(dest,board, WIDTH,HEIGHT);

		tmp = board;
		board = dest;
		dest = tmp;
		
		render(window, tiles, board, WIDTH, HEIGHT);
		SDL_Delay(1000);
	}

	SDL_DestroyWindow(window);
	unload_tiles(tiles);
	SDL_Quit();

	return EXIT_SUCCESS;

err_window:
	unload_tiles(tiles);
err_tiles:
	SDL_Quit();
	free_board(dest, WIDTH, HEIGHT);
err_dest:
	free_board(board, WIDTH, HEIGHT);
err_board:
	return EXIT_FAILURE;
}
