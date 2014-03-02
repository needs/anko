#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "tiles.h"


/* Trick to  get the tile path, used by load_tiles */
#define ADD_TILE(type, path) "./data/tiles/"path,
static const char tiles_path[][64] = {
#include "tiles.def"
};
#undef ADD_TILE


SDL_Texture** load_tiles(SDL_Renderer *renderer)
{
	SDL_Texture** tiles;
	int i;

	assert(renderer != NULL);

	if ((tiles = malloc(TT_COUNT * sizeof(*tiles))) == NULL) {
		perror("malloc(tiles)");
		goto err_tiles;
	}

	for (i = 0; i < TT_COUNT; i++) {
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


void unload_tiles(SDL_Texture **tiles)
{
	int i;
	assert(tiles != NULL);
	for (i = 0; i < TT_COUNT; i++)
		SDL_DestroyTexture(tiles[i]);
	free(tiles);
}

