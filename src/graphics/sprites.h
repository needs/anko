#ifndef _SPRITES_H_
#define _SPRITES_H_

#include <SDL2/SDL.h>


#define TILE_WIDTH  66
#define TILE_HEIGHT 34


/* TT for Tile Type */
#define ADD_SPRITE(type, path) SP_##type,
typedef enum sprite_t {
	SP_NONE = -1,
#include "sprites.def"
	SP_COUNT,
} sprite_t;
#undef ADD_SPRITE


SDL_Texture** load_sprites(SDL_Renderer *renderer);
void unload_sprites(SDL_Texture **sprites);


#endif /* _SPRITES_H_ */
