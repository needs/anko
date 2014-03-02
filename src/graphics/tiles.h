#ifndef _TILES_H_
#define _TILES_H_

#include <SDL2/SDL.h>


#define TILE_WIDTH  66
#define TILE_HEIGHT 34


/* TT for Tile Type */
#define ADD_TILE(type, path) TT_##type,
typedef enum tile_t {
	TT_NONE = -1,
#include "tiles.def"
	TT_COUNT,
} tile_t;
#undef ADD_TILE


SDL_Texture** load_tiles(SDL_Renderer *renderer);
void unload_tiles(SDL_Texture **tiles);


#endif /* _TILES_H_ */
