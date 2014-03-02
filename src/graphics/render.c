#include <assert.h>
#include <SDL2/SDL_image.h>
#include "render.h"


/* Trick to  get the tile path, used by load_tiles */
#define ADD_TILE(type, path) "./data/tiles/"path,
static const char tiles_path[TILE_COUNT][64] = {
#include "tiles.h"
};
#undef ADD_TILE

SDL_Surface** load_tiles()
{
	SDL_Surface** tiles;
	int i;

	if ((tiles = malloc(TILE_COUNT * sizeof(*tiles))) == NULL) {
		perror("malloc(tiles)");
		goto err_tiles;
	}

	for (i = 0; i < TILE_COUNT; i++) {
		if ((tiles[i] = IMG_Load(tiles_path[i])) == NULL) {
			fprintf(stderr, "%s\n", SDL_GetError());
			goto err_tile;
		}
	}

	return tiles;

err_tile:
	while (i --> 0)
		free(tiles[i]);
	free(tiles);
err_tiles:
	return NULL;
}


void render(SDL_Window *window, SDL_Surface **tiles, state_t **board, int width, int height)
{
	assert(window != NULL);
	assert(tiles != NULL);
	assert(board != NULL);
	assert(height > 0);
	assert(width > 0);
}


void unload_tiles(SDL_Surface **tiles)
{
	int i;
	assert(tiles != NULL);
	for (i = 0; i < TILE_COUNT; i++)
		SDL_FreeSurface(tiles[i]);
	free(tiles);
}
