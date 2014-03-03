#include <SDL2/SDL.h>
#include "context.h"
#include "tiles.h"

SDL_Texture  **tiles = NULL;
SDL_Window   *window = NULL;
SDL_Renderer *renderer = NULL;
int quit = 0;

int init_context(void)
{
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
	return 1;

err_tiles:
	SDL_DestroyRenderer(renderer);
err_renderer:
	SDL_DestroyWindow(window);
err_window:
	SDL_Quit();

	return 0;
}


void close_context(void)
{
	unload_tiles(tiles);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

