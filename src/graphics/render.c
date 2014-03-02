#include <assert.h>
#include <SDL2/SDL.h>
#include "render.h"
#include "tiles.h"


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
