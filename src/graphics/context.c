#include <SDL2/SDL.h>
#include "context.h"
#include "sprites.h"

SDL_Texture  **sprites = NULL;
SDL_Window   *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Rect camera = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT};
SDL_Texture *camera_texture = NULL;
unsigned scroll_dir = 0;
float scale=1;
int quit = 0;

int init_context(void)
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Anko",
				   SDL_WINDOWPOS_UNDEFINED, // x
				   SDL_WINDOWPOS_UNDEFINED, // y
				   SCREEN_WIDTH,	    // Width
				   SCREEN_HEIGHT,	    // Height
				   SDL_WINDOW_SHOWN);

	if (!window) {
		fprintf(stderr, "%s\n", SDL_GetError());
		goto err_window;
	}

	renderer = SDL_CreateRenderer(window, -1,
				      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!renderer) {
		fprintf(stderr, "%s\n", SDL_GetError());
		goto err_renderer;
	}

	if ((sprites = load_sprites(renderer)) == NULL)
		goto err_sprites;

	camera_texture = SDL_CreateTexture(renderer,
									   SDL_PIXELFORMAT_ARGB8888,
									   SDL_TEXTUREACCESS_TARGET,
									   SCREEN_WIDTH*ZOOM_MAX_FACTOR,
									   SCREEN_HEIGHT*ZOOM_MAX_FACTOR);
	if(!camera_texture)
		goto err_camera_texture;
	return 1;
	
err_camera_texture:
err_sprites:
	SDL_DestroyRenderer(renderer);
err_renderer:
	SDL_DestroyWindow(window);
err_window:
	SDL_Quit();

	return 0;
}


void close_context(void)
{
	SDL_DestroyTexture(camera_texture);
	unload_sprites(sprites);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

