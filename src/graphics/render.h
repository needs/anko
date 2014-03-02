#ifndef _RENDER_H_
#define _RENDER_H_

#include <SDL2/SDL.h>
#include "../simulator.h"


#define ADD_TILE(type, path) TILE_type,
typedef enum tile_t {
	TILE_NONE = -1,
#include "tiles.h"
	TILE_COUNT,
} tile_t;
#undef ADD_TILE


SDL_Texture** load_tiles(SDL_Renderer *renderer);
void render(SDL_Window *window, SDL_Texture **tiles, state_t **board, int width, int height);
void unload_tiles(SDL_Texture **tiles);


#endif /* _RENDER_H_ */
