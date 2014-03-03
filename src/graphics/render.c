#include <assert.h>
#include <SDL2/SDL.h>
#include "map.h"
#include "context.h"
#include "render.h"
#include "sprites.h"



/* Render the texture at x, y, the texture is centered */
static void render_ctexture(int x, int y, SDL_Texture *tex)
{
	SDL_Rect pos;

	assert(tex != NULL);

	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	pos.x = x - pos.w / 2;
	pos.y = y - pos.h / 2;
	SDL_RenderCopy(renderer, tex, NULL, &pos);
}


void render(board_t *board, map_t *map)
{
	int i, j;

	assert(board != NULL);
	assert(map   != NULL);

	SDL_RenderClear(renderer);

	/* Some sprites might 'overflow' on others on x axis, the real rendering
	 * should go through the map diagonaly, for now, keep it simple. */

	for (i = 0; i < board->height; i++) {
		for (j = 0; j < board->width; j++) {
			int x, y;
			int state;

			/* Note: the camera is centered */
			x = map->cells[i][j].x + camera.pos.x + camera.dim.x / 2;
			y = map->cells[i][j].y + camera.pos.y + camera.dim.y / 2;

			state = board->cells[i][j];

			if (map->cells[i][j].floor[state] != NULL)
				render_ctexture(x, y, map->cells[i][j].floor[state]);
			if (map->cells[i][j].entity[state] != NULL)
				render_ctexture(x, y, map->cells[i][j].entity[state]);
		}
	}

	SDL_RenderPresent(renderer);
}
