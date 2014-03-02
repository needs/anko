#ifndef _TILES_H_
#define _TILES_H_

#include <SDL2/SDL.h>

#define ADD_TILE(type, path) TILE_##type,
typedef enum tile_t {
	TILE_NONE = -1,
#include "tiles.def"
	TILE_COUNT,
} tile_t;
#undef ADD_TILE


SDL_Texture** load_tiles(SDL_Renderer *renderer);
void unload_tiles(SDL_Texture **tiles);


#endif /* _TILES_H_ */
