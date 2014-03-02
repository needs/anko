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
	SDL_Texture **tiles;
	SDL_Window *window;
	SDL_Renderer *renderer;
	state_t **board, **dest, **tmp;

	srandom(time(NULL));
	
	if ((board = generate(WIDTH, HEIGHT)) == NULL)
		goto err_board;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		goto err_dest;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Anko",
				   SDL_WINDOWPOS_UNDEFINED, // x
				   SDL_WINDOWPOS_UNDEFINED, // y
				   640,			    // Width
				   480,			    // Height
				   SDL_WINDOW_SHOWN);

	if (!window) {
		fprintf(stderr, "%s\n", SDL_GetError());
		goto err_window;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED
				      | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "%s\n", SDL_GetError());
		goto err_renderer;
	}

	if ((tiles = load_tiles(renderer)) == NULL)
		goto err_tiles;

	while(1) {
		step(dest,board, WIDTH,HEIGHT);

		tmp = board;
		board = dest;
		dest = tmp;
		
		render(window, tiles, board, WIDTH, HEIGHT);
		SDL_Delay(1000);
	}

	unload_tiles(tiles);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;

err_tiles:
	SDL_DestroyRenderer(renderer);
err_renderer:
	SDL_DestroyWindow(window);
err_window:
	SDL_Quit();
	free_board(dest, WIDTH, HEIGHT);
err_dest:
	free_board(board, WIDTH, HEIGHT);
err_board:
	return EXIT_FAILURE;
}
