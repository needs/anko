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


/* Render the texture at x, y */
static void render_texture(int x, int y, SDL_Texture *tex, SDL_Renderer *rend)
{
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(rend, tex, NULL, &pos);
}


void render(SDL_Renderer *renderer, SDL_Texture **tiles, state_t **board, int width, int height)
{
	int i, j;
	int w, h;

	assert(renderer != NULL);
	assert(tiles != NULL);
	assert(board != NULL);
	assert(height > 0);
	assert(width > 0);

	SDL_RenderClear(renderer);

	/* Some tiles might 'overflow' on others on x axis, the real rendering
	 * should go through the board diagonaly, for now, keep it simple. */

	SDL_QueryTexture(tiles[TILE_GRASS], NULL, NULL, &w, &h);
	w /= 2; h /= 2;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			render_texture((j*-w) + (i*w), i*h + j*h, tiles[TILE_GRASS], renderer);
		}
	}

	SDL_RenderPresent(renderer);
}


void unload_tiles(SDL_Texture **tiles)
{
	int i;
	assert(tiles != NULL);
	for (i = 0; i < TILE_COUNT; i++)
		SDL_DestroyTexture(tiles[i]);
	free(tiles);
}
