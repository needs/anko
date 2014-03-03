#include <assert.h>
#include <SDL2/SDL.h>
#include "context.h"
#include "render.h"
#include "tiles.h"



/* Render the texture at x, y, the texture is centered */
static void render_ctexture(int x, int y, SDL_Texture *tex)
{
	SDL_Rect pos;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	pos.x = x - pos.w / 2;
	pos.y = y - pos.h / 2;
	SDL_RenderCopy(renderer, tex, NULL, &pos);
}


void render(board_t *board)
{
	int i, j;

	assert(board != NULL);

	SDL_RenderClear(renderer);

	/* Some tiles might 'overflow' on others on x axis, the real rendering
	 * should go through the board diagonaly, for now, keep it simple. */

	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			int x, y;

			/* Note: the camera is centered */
			x = camera.x + camera.w / 2;
			y = camera.y + camera.h / 2;
			x += j*-TILE_WIDTH/2 + i*TILE_WIDTH/2;
			y += i*TILE_HEIGHT/2 + j*TILE_HEIGHT/2;

			if (board->cells[i][j] == ST_WATER)
				render_ctexture(x, y, tiles[TT_WATER]);
			else
				render_ctexture(x, y, tiles[TT_GRASS]);

			if (board->cells[i][j] == ST_BURNABLE)
				render_ctexture(x, y, tiles[TT_TREE]);
			else if (board->cells[i][j] == ST_BURNED)
				render_ctexture(x, y, tiles[TT_BURNED_TREE]);
			else if (board->cells[i][j] == ST_BURNING)
				render_ctexture(x, y, tiles[TT_BURNING_TREE]);
		}
	}

	SDL_RenderPresent(renderer);
}
