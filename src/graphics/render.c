#include <assert.h>
#include <SDL2/SDL.h>
#include "render.h"
#include "tiles.h"



/* Render the texture at x, y, the texture is centered */
static void render_ctexture(int x, int y, SDL_Texture *tex, SDL_Renderer *rend)
{
	SDL_Rect pos;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	pos.x = x - pos.w / 2;
	pos.y = y - pos.h / 2;
	SDL_RenderCopy(rend, tex, NULL, &pos);
}


void render(SDL_Renderer *renderer, SDL_Texture **tiles, state_t **board, int width, int height)
{
	int i, j;

	assert(renderer != NULL);
	assert(tiles != NULL);
	assert(board != NULL);
	assert(height > 0);
	assert(width > 0);

	SDL_RenderClear(renderer);

	/* Some tiles might 'overflow' on others on x axis, the real rendering
	 * should go through the board diagonaly, for now, keep it simple. */

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			render_ctexture((j*-TILE_WIDTH/2) + (i*TILE_WIDTH/2), i*TILE_HEIGHT/2 + j*TILE_HEIGHT/2, tiles[TT_GRASS], renderer);
		}
	}

	SDL_RenderPresent(renderer);
}
