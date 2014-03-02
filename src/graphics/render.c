#include <assert.h>
#include <SDL2/SDL_image.h>
#include "render.h"


/* Trick to  get the tile path, used by load_tiles */
#define ADD_TILE(type, path) "./data/tiles/"path,
static const char tiles_path[TILE_COUNT][64] = {
#include "tiles.h"
};
#undef ADD_TILE

SDL_Texture** load_tiles(SDL_Renderer *renderer)
{
	SDL_Texture** tiles;
	int i;

	assert(renderer != NULL);

	if ((tiles = malloc(TILE_COUNT * sizeof(*tiles))) == NULL) {
		perror("malloc(tiles)");
		goto err_tiles;
	}

	for (i = 0; i < TILE_COUNT; i++) {
		SDL_Surface *surf = IMG_Load(tiles_path[i]);
		if (surf == NULL) {
			fprintf(stderr, "%s\n", SDL_GetError());
			goto err_tile;
		}
		tiles[i] = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
	}

	return tiles;

err_tile:
	while (i --> 0)
		SDL_DestroyTexture(tiles[i]);
	free(tiles);
err_tiles:
	return NULL;
}


void render(SDL_Window *window, SDL_Texture **tiles, state_t **board, int width, int height)
{
	assert(window != NULL);
	assert(tiles != NULL);
	assert(board != NULL);
	assert(height > 0);
	assert(width > 0);
}


void unload_tiles(SDL_Texture **tiles)
{
	int i;
	assert(tiles != NULL);
	for (i = 0; i < TILE_COUNT; i++)
		SDL_DestroyTexture(tiles[i]);
	free(tiles);
}
